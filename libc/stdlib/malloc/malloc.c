/*
  malloc - heap manager based on heavy use of virtual memory management.
  Copyright (C) 1998   Valery Shchedrin

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
  MA 02111-1307, USA
  
  Public Functions:
  
  void *malloc(size_t size);
  
    Allocates `size` bytes
    returns NULL if no free memory available
  
  void *calloc(size_t unit, size_t quantity);
  
    Allocates `quantity*unit` zeroed bytes via internal malloc call
  
  void *realloc(void *ptr, size_t size);
  
    Reallocates already allocated block `ptr`, if `ptr` is not valid block
    then it works as malloc. NULL is returned if no free memory available
  
  void *_realloc_no_move(void *ptr, size_t size);
  
    Reallocates already allocated block `ptr`, if `ptr` is not valid block
    or if reallocation can't be done with shrinking/expanding already
    allocated block NULL is returned
  
  void free(void *ptr);
  
    Frees already allocated block, if `ptr` is incorrect one nothing will
    happen.
*/

/*
 * Manuel Novoa III         Jan 2001
 *
 * Modified to decrease object sizes.
 *   Broke into independent object files.
 *   Converted INIT_BLOCK() and FREE_MEM_DEL_BLOCK() from macros to functions.
 */

#define _POSIX_SOURCE
#define _XOPEN_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <sys/time.h>
#include <asm/page.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "malloc.h"


#define M_DOTRIMMING 1
#define M_MULTITHREADED 0

#define VALLOC_MSTART  ((void*)0x1c000000)
#define LARGE_MSTART   ((void*)0x19000000)
#define HUNK_MSTART    ((void*)0x18000000)
#define HUNK_MSIZE     M_PAGESIZE
#define HUNK_ID        0x99171713

/* alignment of allocations > HUNK_THRESHOLD */
#define MALLOC_ALIGN    4

/* allocations < HUNK_THRESHOLD will not be aligned */
#define HUNK_THRESHOLD  4

/*up to HUNK_MAXSIZE blocks will be joined together to decrease memory waste*/
#define HUNK_MAXSIZE 128

/* returns value not less than size, aligned to MALLOC_ALIGN */
#define ALIGN(size) (((size)+(MALLOC_ALIGN)-1)&(~((MALLOC_ALIGN)-1)))

/* aligns s or p to page boundaries */
#define PAGE_ALIGN(s) (((s)+M_PAGESIZE-1)&(~(M_PAGESIZE-1)))
#define PAGE_ALIGNP(p) ((char*)PAGE_ALIGN((unsigned)(p)))
#define PAGE_DOWNALIGNP(p) ((char*)(((unsigned)(p))&(~(M_PAGESIZE-1))))

/* returns v * 2 for your machine (speed-up) */
#define MUL2(v)  ((v)*2)

/* does v *= 8 for your machine (speed-up) */
#define EMUL8(v) v*=8

/* does v/8 for your machind (speed-up) */
#define DIV8(v)  ((v)/8)

#if M_MULTITHREADED
#error This version does not support threads
#else
typedef int mutex_t;

#define mutex_lock(x)
#define mutex_unlock(x)
#define mutex_init(x)
#define MUTEX_INITIALIZER 0
//static mutex_t malloc_lock = MUTEX_INITIALIZER;
#endif

extern int __malloc_initialized;

#ifdef L__malloc_init
int __malloc_initialized = -1;

 /* -1 == uninitialized, 0 == initializing, 1 == initialized */
#endif

#ifndef MAP_FAILED
#define MAP_FAILED ((void*)-1)
#endif

#if defined(MAP_ANONYMOUS) && !defined(MAP_ANON)
#define MAP_ANON MAP_ANONYMOUS
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

/* guess pagesize */
#ifndef M_PAGESIZE
#ifdef _SC_PAGESIZE
#ifndef _SC_PAGE_SIZE
#define _SC_PAGE_SIZE _SC_PAGESIZE
#endif
#endif
#ifdef _SC_PAGE_SIZE
#define M_PAGESIZE sysconf(_SC_PAGE_SIZE)
#else							/* !_SC_PAGESIZE */
#if defined(BSD) || defined(DGUX) || defined(HAVE_GETPAGESIZE)
extern size_t getpagesize();

#define M_PAGESIZE getpagesize()
#else							/* !HAVE_GETPAGESIZE */
#include <sys/param.h>
#ifdef EXEC_PAGESIZE
#define M_PAGESIZE EXEC_PAGESIZE
#else							/* !EXEC_PAGESIZE */
#ifdef NBPG
#ifndef CLSIZE
#define M_PAGESIZE NBPG
#else							/* !CLSIZE */
#define M_PAGESIZE (NBPG*CLSIZE)
#endif							/* CLSIZE */
#else
#ifdef NBPC
#define M_PAGESIZE NBPC
#else							/* !NBPC */
#ifdef PAGESIZE
#define M_PAGESIZE PAGESIZE
#else							/* !PAGESIZE */
#define M_PAGESIZE 4096
#endif							/* PAGESIZE */
#endif							/* NBPC */
#endif							/* NBPG */
#endif							/* EXEC_PAGESIZE */
#endif							/* HAVE_GETPAGESIZE */
#endif							/* _SC_PAGE_SIZE */
#endif							/* defined(M_PAGESIZE) */

/* HUNK MANAGER */

typedef struct Hunk_s Hunk_t;

struct Hunk_s {					/* Hunked block - 8 byte overhead */
	int id;						/* unique id */
	unsigned int total:12, used:12, size:8;
	Hunk_t *next;				/* next free in __free_h */
};

#define usagemap(h) (((unsigned char *)(h))+sizeof(Hunk_t))
#define hunk_ptr(h) (((char*)(h))+sizeof(Hunk_t)+ALIGN(DIV8(h->total+7)))
#define hunk(h)  ((Hunk_t*)(h))

extern Hunk_t *__free_h[HUNK_MAXSIZE + 1];
extern int __total_h[HUNK_MAXSIZE + 1];

#ifdef L__malloc_init
Hunk_t *__free_h[HUNK_MAXSIZE + 1];	/* free hash */
int __total_h[HUNK_MAXSIZE + 1];	/* Hunk_t's `total` member */
#endif

extern void *__hunk_alloc(int size);

#ifdef L_malloc
/* __hunk_alloc allocates <= HUNK_MAXSIZE blocks */
void *__hunk_alloc(int size)
{
	Hunk_t *p;
	unsigned long *cpl;
	int i, c;

	if (size >= HUNK_THRESHOLD)
		size = ALIGN(size);

	/* Look for already allocated hunkblocks */
	if ((p = __free_h[size]) == NULL) {
		if (
			(p =
			 (Hunk_t *) mmap(HUNK_MSTART, HUNK_MSIZE,
							 PROT_READ | PROT_WRITE,
#ifdef __HAS_NO_MMU__
							 MAP_SHARED | MAP_ANONYMOUS
#else
							 MAP_PRIVATE | MAP_ANONYMOUS
#endif
							 , 0, 0)) == (Hunk_t *) MAP_FAILED)
			return NULL;
		memset(p, 0, HUNK_MSIZE);
		p->id = HUNK_ID;
		p->total = __total_h[size];
		/* p->used = 0; */
		p->size = size;
		/* p->next = (Hunk_t*)NULL; */
		/* memset(usagemap(p), 0, bound); */
		__free_h[size] = p;
	}

	/* Locate free point in usagemap */
	for (cpl = (unsigned long *) usagemap(p); *cpl == 0xFFFFFFFF; cpl++);
	i = ((unsigned char *) cpl) - usagemap(p);
	if (*(unsigned short *) cpl != 0xFFFF) {
		if (*(unsigned char *) cpl == 0xFF) {
			c = *(int *) (((unsigned char *) cpl) + 1);
			i++;
		} else
			c = *(int *) (unsigned char *) cpl;
	} else {
		i += 2;
		c = *(((unsigned char *) cpl) + 2);
		if (c == 0xFF) {
			c = *(int *) (((unsigned char *) cpl) + 3);
			i++;
		}
	}
	EMUL8(i);
	if ((c & 0xF) == 0xF) {
		c >>= 4;
		i += 4;
	}
	if ((c & 0x3) == 0x3) {
		c >>= 2;
		i += 2;
	}
	if (c & 1)
		i++;

	usagemap(p)[DIV8(i)] |= (1 << (i & 7));	/* set bit */

	/* Increment counter and update hashes */
	if (++p->used == p->total) {
		__free_h[p->size] = p->next;
		p->next = NULL;
	}
	return hunk_ptr(p) + i * p->size;
}
#endif							/* L_malloc */

extern void __hunk_free(char *ptr);

#ifdef L__free_support
/* __hunk_free frees blocks allocated by __hunk_alloc */
void __hunk_free(char *ptr)
{
	unsigned char *up;
	int i, v;
	Hunk_t *h;

	if (!ptr)
		return;

	h = (Hunk_t *) PAGE_DOWNALIGNP(ptr);

	/* Validate `ptr` */
	if (h->id != HUNK_ID)
		return;
	v = ptr - hunk_ptr(h);
	i = v / h->size;
	if (v % h->size != 0 || i < 0 || i >= h->total)
		return;

	/* Update `usagemap` */
	up = &(usagemap(h)[DIV8(i)]);
	i = 1 << (i & 7);
	if (!(*up & i))
		return;
	*up ^= i;

	/* Update hunk counters */
	if (h->used == h->total) {
		if (--h->used) {		/* insert into __free_h */
			h->next = __free_h[h->size];
			__free_h[h->size] = h;
		}						/* else - it will be unmapped */
	} else {
		if (!--h->used) {		/* delete from __free_h - will be __bl_freed */
			Hunk_t *p, *pp;

			for (p = __free_h[h->size], pp = NULL; p != h;
				 pp = p, p = p->next);
			if (!pp)
				__free_h[h->size] = p->next;
			else
				pp->next = p->next;
		}
	}

	/* Unmap empty Hunk_t */
	if (!h->used)
		munmap((void *) h, HUNK_MSIZE);
}
#endif							/* L__free_support */

/* BLOCK MANAGER */

typedef struct Block_s Block_t;

struct Block_s {				/* 32-bytes long control structure (if 4-byte aligned) */
	char *ptr;					/* pointer to related data */
	Block_t *next;				/* next in free_mem list */
	Block_t *l_free_mem, *r_free_mem;	/* left & right subtrees of <free_mem> */
	Block_t *l_ptrs, *r_ptrs;	/* left & right subtrees of <ptrs> */
	size_t size;				/* size - divided by align */

	/* packed 4-byte attributes */
/* { */
	signed char bal_free_mem:8;	/* balance of <free_mem> subtree */
	signed char bal_ptrs:8;		/* balance of <ptrs> subtree */
	unsigned int used:1;		/* used/free state of the block */
	unsigned int broken:1;		/* 1 if previous block can't be merged with it */
/* } */
};

extern Block_t *__bl_last;		/* last mmapped block */

#ifdef L__malloc_init
Block_t *__bl_last;				/* last mmapped block */
#endif

#define bl_get() __hunk_alloc(sizeof(Block_t))
#define bl_rel(p) __hunk_free((char*)p)

extern Block_t *__Avl_Block_tfree_mem_tree;
extern Block_t *__free_mem_ins(Block_t * data);
extern void __free_mem_del(Block_t * data);
extern void __free_mem_replace(Block_t * data);
extern Block_t *__Avl_Block_tptrs_tree;
extern Block_t *__ptrs_ins(Block_t * data);
extern void __ptrs_del(Block_t * data);

extern void __bl_uncommit(Block_t * b);
extern void __bl_free(Block_t * b);

/* like C++ templates ;-) */
#include "avlmacro.h"

#define FREE_MEM_COMPARE(i,a,b) \
{ \
  if ( (a)->size < (b)->size ) { \
     i = -1; \
  } else if ( (a)->size > (b)->size ) { \
     i = 1; \
  } else { \
     i = 0; \
  } \
}

#define PTRS_COMPARE(i,a,b) \
{ \
  if ( (a)->ptr < (b)->ptr ) { \
     i = -1; \
  } else if ( (a)->ptr > (b)->ptr ) { \
     i = 1; \
  } else { \
     i = 0; \
  } \
}

#ifdef L__avl_support
Avl_Tree(free_mem, Block_t, free_mem, FREE_MEM_COMPARE)
	Avl_Tree_no_replace(ptrs, Block_t, ptrs, PTRS_COMPARE)
#endif
#define free_mem_root Avl_Root(Block_t, free_mem)
#define ptrs_root Avl_Root(Block_t, ptrs)
/* pp is freed block */
#define FREE_MEM_DEL_BLOCK(pp,p) {p = __free_mem_del_block(pp,p);}
extern Block_t *__free_mem_del_block(Block_t * pp, Block_t * p);

#ifdef L_malloc
Block_t *__free_mem_del_block(Block_t * pp, Block_t * p)
{
	for (p = free_mem_root;;)
		if (p->size > pp->size)
			p = p->l_free_mem;
		else if (p->size < pp->size)
			p = p->r_free_mem;
		else
			break;
	if (p == pp) {
		if (pp->next)
			__free_mem_replace(pp->next);
		else
			__free_mem_del(pp);
	} else {
		for (; p->next != pp; p = p->next);
		p->next = pp->next;
	}
	return p;
}
#endif							/* L_malloc */

#define FREE_MEM_INS_BLOCK(pp) \
{ \
  if ((p = __free_mem_ins(pp)) != NULL)\
  {\
    pp->next = p->next;\
    p->next = pp;\
  }\
  else pp->next = NULL; \
}

/* `b` is current block, `pp` is next block */
#define COMBINE_BLOCKS(b,pp) \
{\
  __ptrs_del(pp); \
  b->size += pp->size; \
  if (pp == __bl_last) __bl_last = b; \
  bl_rel(pp); \
}

/* initializes new block b */
#define INIT_BLOCK(b, pppp, sz) { p = __init_block(b, pppp, sz); }

extern Block_t *__init_block(Block_t * b, char *pppp, size_t sz);

#ifdef L_malloc
Block_t *__init_block(Block_t * b, char *pppp, size_t sz)
{
	Block_t *p;

	memset(b, 0, sizeof(Block_t));
	b->ptr = pppp;
	b->size = sz;
	__ptrs_ins(b);
	FREE_MEM_INS_BLOCK(b);
	return p;
}
#endif							/* L_malloc */

/* `b` is current block, `sz` its new size */
/* block `b` will be splitted to one busy & one free block */
#define SPLIT_BLOCK(b,sz) \
{\
  Block_t *bt; \
  bt = bl_get(); \
  INIT_BLOCK(bt, b->ptr + sz, b->size - sz); \
  b->size = sz; \
  if (__bl_last == b) __bl_last = bt; \
  __bl_uncommit(bt);\
}

/* `b` is current block, `pp` is next free block, `sz` is needed size */
#define SHRINK_BLOCK(b,pp,sz) \
{\
  FREE_MEM_DEL_BLOCK(pp,p); \
  pp->ptr = b->ptr + sz; \
  pp->size += b->size - sz; \
  b->size = sz; \
  FREE_MEM_INS_BLOCK(pp); \
  __bl_uncommit(pp); \
}

#ifdef L_malloc
static Block_t *bl_mapnew(size_t size)
{
	size_t map_size;
	Block_t *pp, *p;
	void *pt;

	map_size = PAGE_ALIGN(size);
	pt = mmap(LARGE_MSTART, map_size, PROT_READ | PROT_WRITE | PROT_EXEC,
#ifdef __HAS_NO_MMU__
							 MAP_SHARED | MAP_ANONYMOUS
#else
							 MAP_PRIVATE | MAP_ANONYMOUS
#endif
							 , 0, 0);

	if (pt == MAP_FAILED)
		return (Block_t *) NULL;

	__bl_last = pp = bl_get();
	INIT_BLOCK(pp, (char *) pt, map_size);
	pp->broken = 1;

	return pp;
}

void __bl_uncommit(Block_t * b)
{
	char *u_start, *u_end;

	u_start = PAGE_ALIGNP(b->ptr);
	u_end = PAGE_DOWNALIGNP(b->ptr + b->size);
	if (u_end <= u_start)
		return;

#if M_DOTRIMMING
	mmap(u_start, u_end - u_start, PROT_READ | PROT_WRITE | PROT_EXEC,
#ifdef __HAS_NO_MMU__
							 MAP_SHARED | MAP_ANONYMOUS |MAP_FIXED
#else
							 MAP_PRIVATE | MAP_ANONYMOUS |MAP_FIXED
#endif
							 , 0, 0);
#endif
}

/* requested size must be aligned to ALIGNMENT */
static Block_t *bl_alloc(size_t size)
{
	Block_t *p, *pp;

	/* try to find needed space in existing memory */
	for (p = free_mem_root, pp = NULL; p;) {
		if (p->size > size) {
			pp = p;
			p = p->l_free_mem;
		} else if (p->size < size)
			p = p->r_free_mem;
		else {
			pp = p;
			break;
		}
	}

	if (!pp) {					/* map some memory */
		if (!__bl_last) {		/* just do initial mmap */
			pp = bl_mapnew(size);
			if (!pp)
				return NULL;
		} else if (!__bl_last->used) {	/* try growing last unused */
			if (mremap(PAGE_DOWNALIGNP(__bl_last->ptr),
					   PAGE_ALIGNP(__bl_last->ptr + __bl_last->size) -
					   PAGE_DOWNALIGNP(__bl_last->ptr),
					   PAGE_ALIGNP(__bl_last->ptr + size) -
					   PAGE_DOWNALIGNP(__bl_last->ptr), 0) == MAP_FAILED) {	/* unable to grow -- initiate new block */
				pp = bl_mapnew(size);
				if (!pp)
					return NULL;
			} else {
				pp = __bl_last;
				FREE_MEM_DEL_BLOCK(pp, p);
				pp->size = PAGE_ALIGNP(pp->ptr + size) - pp->ptr;
				FREE_MEM_INS_BLOCK(pp);
			}
		} else {				/* __bl_last is used block */
			if (mremap(PAGE_DOWNALIGNP(__bl_last->ptr),
					   PAGE_ALIGNP(__bl_last->ptr + __bl_last->size) -
					   PAGE_DOWNALIGNP(__bl_last->ptr),
					   PAGE_ALIGNP(__bl_last->ptr + __bl_last->size +
								   size) - PAGE_DOWNALIGNP(__bl_last->ptr),
					   0) == MAP_FAILED) {
				pp = bl_mapnew(size);
				if (!pp)
					return NULL;
			} else {
				pp = bl_get();
				INIT_BLOCK(pp, __bl_last->ptr + __bl_last->size,
						   PAGE_ALIGNP(__bl_last->ptr + __bl_last->size +
									   size) - __bl_last->ptr -
						   __bl_last->size);
				__bl_last = pp;
			}
		}
	}

	/* just delete this node from free_mem tree */
	if (pp->next)
		__free_mem_replace(pp->next);
	else
		__free_mem_del(pp);
	pp->used = 1;

	if (pp->size - size > MALLOC_ALIGN) {	/* this block can be splitted (it is unused,not_broken) */
		SPLIT_BLOCK(pp, size);
	}

	return pp;
}
#endif							/* L_malloc */

#ifdef L__free_support
void __bl_free(Block_t * b)
{
	Block_t *p, *bl_next, *bl_prev;

	/* Look for blocks before & after `b` */
	for (p = ptrs_root, bl_next = NULL, bl_prev = NULL; p;) {
		if (p->ptr > b->ptr) {
			bl_next = p;
			p = p->l_ptrs;
		} else if (p->ptr < b->ptr) {
			bl_prev = p;
			p = p->r_ptrs;
		} else
			break;
	}
	if (b->l_ptrs)
		for (bl_prev = b->l_ptrs; bl_prev->r_ptrs;
			 bl_prev = bl_prev->r_ptrs);
	if (b->r_ptrs)
		for (bl_next = b->r_ptrs; bl_next->l_ptrs;
			 bl_next = bl_next->l_ptrs);

	if (bl_next && !bl_next->broken && !bl_next->used) {
		FREE_MEM_DEL_BLOCK(bl_next, p)
			COMBINE_BLOCKS(b, bl_next)
	}

	if (bl_prev && !b->broken && !bl_prev->used) {
		FREE_MEM_DEL_BLOCK(bl_prev, p)
			COMBINE_BLOCKS(bl_prev, b)
			b = bl_prev;
	}

	b->used = 0;
	FREE_MEM_INS_BLOCK(b)
		__bl_uncommit(b);
}
#endif							/* L__free_support */

extern void __malloc_init(void);

#ifdef L__malloc_init
void __malloc_init(void)
{
	int i, mapsize, x, old_x, gcount;

	mapsize = M_PAGESIZE;

	__malloc_initialized = 0;
	__bl_last = NULL;
	free_mem_root = NULL;
	ptrs_root = NULL;
	mapsize -= sizeof(Hunk_t);
	for (i = 1; i <= HUNK_MAXSIZE; i++) {
		__free_h[i] = (Hunk_t *) NULL;
		for (x = mapsize / i, gcount = 0, old_x = 0; old_x != x;) {
			old_x = x;
			x = (mapsize - ALIGN(DIV8(old_x + 7))) / i;
			if (gcount > 1 && x * i + ALIGN(DIV8(x + 7)) <= mapsize)
				break;
			if (x * i + ALIGN(DIV8(x + 7)) > mapsize)
				gcount++;
		}
		__total_h[i] = x;
	}
	mutex_init(&malloc_lock);
	__malloc_initialized = 1;
}
#endif							/* L__malloc_init */

#ifdef L_malloc
void *malloc(size_t size)
{
	void *p;

	if (size == 0)
		return NULL;

	if (__malloc_initialized < 0)
		__malloc_init();
	if (__malloc_initialized)
		mutex_lock(&malloc_lock);

	if (size <= HUNK_MAXSIZE)
		p = __hunk_alloc(size);
	else {
		if ((p = bl_alloc(ALIGN(size))) != NULL)
			p = ((Block_t *) p)->ptr;
	}

	if (__malloc_initialized)
		mutex_unlock(&malloc_lock);

	return p;
}
#endif							/* L_malloc */

#ifdef L_free
void free(void *ptr)
{
	Block_t *p, *best;

	if (__malloc_initialized < 0)
		return;
	if (__malloc_initialized)
		mutex_lock(&malloc_lock);

	for (p = ptrs_root, best = NULL; p;) {
		if (p->ptr > (char *) ptr)
			p = p->l_ptrs;
		else {
			best = p;
			p = p->r_ptrs;
		}
	}

	if (!best || !best->used || best->ptr != (char *) ptr) {
		__hunk_free(ptr);
		if (__malloc_initialized)
			mutex_unlock(&malloc_lock);
		return;
	}

	__bl_free(best);

	if (__malloc_initialized)
		mutex_unlock(&malloc_lock);
}
#endif							/* L_free */

extern void *_realloc_no_move(void *ptr, size_t size);

#ifdef L__realloc_no_move
void *_realloc_no_move(void *ptr, size_t size)
{
	Block_t *p, *best, *next;

	if (size <= HUNK_MAXSIZE)
		return NULL;

	if (__malloc_initialized <= 0)
		return malloc(size);

	mutex_lock(&malloc_lock);

	/* Locate block */
	for (p = ptrs_root, best = NULL; p;) {
		if (p->ptr > (char *) ptr)
			p = p->l_ptrs;
		else {
			best = p;
			p = p->r_ptrs;
		}
	}

	if (!best || !best->used || best->ptr != (char *) ptr) {
		mutex_unlock(&malloc_lock);
		return NULL;
	}

	size = ALIGN(size);

	if (size == best->size) {
		mutex_unlock(&malloc_lock);
		return ptr;
	}

	if (best->r_ptrs)			/* get block just after */
		for (next = best->r_ptrs; next->l_ptrs; next = next->l_ptrs);
	else
		for (p = ptrs_root, next = NULL; p;) {
			if (p->ptr > best->ptr) {
				next = p;
				p = p->l_ptrs;
			} else if (p->ptr < best->ptr)
				p = p->r_ptrs;
			else
				break;
		}

	if (size < best->size) {	/* shrink block */
		if (!next || next->used || next->broken) {
			if (best->size - size > MALLOC_ALIGN) {	/* do split */
				SPLIT_BLOCK(best, size);
			}
		} else {				/* just move border of next block */
			SHRINK_BLOCK(best, next, size);
		}
	} else if (next && !next->broken && !next->used) {	/* can expand */
		if (best->size + next->size > size + HUNK_MAXSIZE) {	/* shrink next free block */
			SHRINK_BLOCK(best, next, size);
		} else if (best->size + next->size >= size) {	/* combine blocks (eat next one) */
			FREE_MEM_DEL_BLOCK(next, p);
			COMBINE_BLOCKS(best, next);
		} else {				/* not enough memory in next block */
			mutex_unlock(&malloc_lock);
			return NULL;
		}
	} else {					/* no next block */
		mutex_unlock(&malloc_lock);
		return NULL;
	}
	mutex_unlock(&malloc_lock);
	return best->ptr;
}
#endif							/* L__realloc_no_move */

#ifdef L_realloc
void *realloc(void *ptr, size_t size)
{
	void *tmp;

	tmp = _realloc_no_move(ptr, size);

	if (!tmp) {
		Block_t *p, *best;

		mutex_lock(&malloc_lock);

		for (p = ptrs_root, best = NULL; p;) {
			if (p->ptr > (char *) ptr)
				p = p->l_ptrs;
			else {
				best = p;
				p = p->r_ptrs;
			}
		}

		if (!best || !best->used || best->ptr != (char *) ptr) {
			if (ptr) {
				Hunk_t *h;

				h = (Hunk_t *) PAGE_DOWNALIGNP(ptr);
				if (h->id == HUNK_ID) {
					mutex_unlock(&malloc_lock);
					if ((size >= HUNK_THRESHOLD && ALIGN(size) == h->size)
						|| size == h->size)
						return ptr;
					if ((tmp = malloc(size)) == NULL)
						return NULL;
					mutex_lock(&malloc_lock);
					memcpy(tmp, ptr, ((size < h->size) ? size : h->size));
					__hunk_free(ptr);
					mutex_unlock(&malloc_lock);
					return tmp;
				}
			}
			mutex_unlock(&malloc_lock);
			return malloc(size);
		}

		mutex_unlock(&malloc_lock);

		/* copy whole block */
		if ((tmp = malloc(size)) == NULL)
			return NULL;
		memcpy(tmp, ptr, ((size < best->size) ? size : best->size));

		mutex_lock(&malloc_lock);
		__bl_free(best);
		mutex_unlock(&malloc_lock);
	}
	return tmp;
}
#endif							/* L_realloc */

#ifdef L_calloc
void *calloc(size_t unit, size_t quantity)
{
	void *p;

	unit *= quantity;

	if ((p = malloc(unit)) == NULL)
		return NULL;
	memset(p, 0, unit);
	return p;
}
#endif							/* L_calloc */
