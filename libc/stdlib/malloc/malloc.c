/*
  mmalloc - heap manager based on heavy use of virtual memory management.
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
  
  void *mmalloc(size_t size);
  
    Allocates `size` bytes
    returns NULL if no free memory available
  
  void *mcalloc(size_t unit, size_t quantity);
  
    Allocates `quantity*unit` zeroed bytes via internal malloc call
  
  void *mrealloc(void *ptr, size_t size);
  
    Reallocates already allocated block `ptr`, if `ptr` is not valid block
    then it works as malloc. NULL is returned if no free memory available
  
  void *mrealloc_no_move(void *ptr, size_t size);
  
    Reallocates already allocated block `ptr`, if `ptr` is not valid block
    or if reallocation can't be done with shrinking/expanding already
    allocated block NULL is returned
  
  void mfree(void *ptr);
  
    Frees already allocated block, if `ptr` is incorrect one nothing will
    happen.
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

static int mmalloc_initialized = -1;
 /* -1 == uninitialized, 0 == initializing, 1 == initialized */

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
 #else /* !_SC_PAGESIZE */
  #if defined(BSD) || defined(DGUX) || defined(HAVE_GETPAGESIZE)
   extern size_t getpagesize();
   #define M_PAGESIZE getpagesize()
  #else /* !HAVE_GETPAGESIZE */
   #include <sys/param.h>
   #ifdef EXEC_PAGESIZE
    #define M_PAGESIZE EXEC_PAGESIZE
   #else /* !EXEC_PAGESIZE */
    #ifdef NBPG
     #ifndef CLSIZE
      #define M_PAGESIZE NBPG
     #else /* !CLSIZE */
      #define M_PAGESIZE (NBPG*CLSIZE)
     #endif /* CLSIZE */
    #else
     #ifdef NBPC
      #define M_PAGESIZE NBPC
     #else /* !NBPC */
      #ifdef PAGESIZE
       #define M_PAGESIZE PAGESIZE
      #else /* !PAGESIZE */
       #define M_PAGESIZE 4096
      #endif /* PAGESIZE */
     #endif /* NBPC */
    #endif /* NBPG */
   #endif /* EXEC_PAGESIZE */
  #endif /* HAVE_GETPAGESIZE */
 #endif /* _SC_PAGE_SIZE */
#endif /* defined(M_PAGESIZE) */

/* HUNK MANAGER */

typedef struct Hunk_s  Hunk_t;

struct Hunk_s { /* Hunked block - 8 byte overhead */
  int id; /* unique id */
  unsigned int total:12, used:12, size : 8;
  Hunk_t *next; /* next free in free_h */
};

static Hunk_t *free_h[HUNK_MAXSIZE+1]; /* free hash */
int total_h[HUNK_MAXSIZE+1]; /* Hunk_t's `total` member */

#define usagemap(h) (((unsigned char *)(h))+sizeof(Hunk_t))
#define hunk_ptr(h) (((char*)(h))+sizeof(Hunk_t)+ALIGN(DIV8(h->total+7)))
#define hunk(h)  ((Hunk_t*)(h))

/* hunk_alloc allocates <= HUNK_MAXSIZE blocks */
static void *hunk_alloc(int size)
{
  Hunk_t *p;
  unsigned long *cpl;
  int i, c;
  
  if (size >= HUNK_THRESHOLD) size = ALIGN(size);
  
  /* Look for already allocated hunkblocks */
  if ((p = free_h[size]) == NULL)
  {
    if ((p = (Hunk_t*)mmap(HUNK_MSTART,HUNK_MSIZE,PROT_READ|PROT_WRITE,
#ifdef __HAS_NO_MMU__
	MAP_SHARED|MAP_ANONYMOUS
#else
	MAP_PRIVATE|MAP_ANONYMOUS
#endif
	,0,0)) == (Hunk_t*)MAP_FAILED)
      return NULL;
    memset(p,0,HUNK_MSIZE);
    p->id = HUNK_ID;
    p->total = total_h[size];
    /* p->used = 0; */
    p->size = size;
    /* p->next = (Hunk_t*)NULL; */
    /* memset(usagemap(p), 0, bound); */
    free_h[size] = p;
  }

  /* Locate free point in usagemap */
  for (cpl=(unsigned long*)usagemap(p);*cpl==0xFFFFFFFF;cpl++);
  i = ((unsigned char *)cpl) - usagemap(p);
  if (*(unsigned short*)cpl != 0xFFFF) {
    if (*(unsigned char*)cpl == 0xFF) {
      c = *(int*)(((unsigned char *)cpl)+1); i++;
      } else  c = *(int*)(unsigned char *)cpl;
  } else {
    i+=2; c = *(((unsigned char *)cpl)+2);
    if (c == 0xFF) { c = *(int*)(((unsigned char *)cpl)+3); i++; }
  }
  EMUL8(i);
  if ((c & 0xF) == 0xF) { c >>= 4; i+=4; }
  if ((c & 0x3) == 0x3) { c >>= 2; i+=2; }
  if (c & 1) i++;
  
  usagemap(p)[DIV8(i)] |= (1 << (i & 7)); /* set bit */
  
  /* Increment counter and update hashes */
  if (++p->used == p->total)
  {
    free_h[p->size] = p->next;
    p->next = NULL;
  }
  return hunk_ptr(p)+i*p->size;
}

/* hunk_free frees blocks allocated by hunk_alloc */
static void hunk_free(char *ptr)
{
  unsigned char *up;
  int i, v;
  Hunk_t *h;
  
  if (!ptr) return;
  
  h = (Hunk_t*)PAGE_DOWNALIGNP(ptr);
  
  /* Validate `ptr` */
  if (h->id != HUNK_ID) return;
  v = ptr - hunk_ptr(h);
  i = v / h->size;
  if (v % h->size != 0 || i < 0 || i >= h->total) return;
  
  /* Update `usagemap` */
  up = &(usagemap(h)[DIV8(i)]);
  i = 1 << (i&7);
  if (!(*up & i)) return;
  *up ^= i;
  
  /* Update hunk counters */
  if (h->used == h->total)
  {
    if (--h->used)
    { /* insert into free_h */
      h->next = free_h[h->size];
      free_h[h->size] = h;
    } /* else - it will be unmapped */
  }
  else
  {
    if (!--h->used)
    { /* delete from free_h - will be bl_freed*/
      Hunk_t *p, *pp;
      for (p=free_h[h->size],pp=NULL;p!=h;pp=p,p=p->next);
      if (!pp)
	free_h[h->size] = p->next;
      else
	pp->next = p->next;
    }
  }
  
  /* Unmap empty Hunk_t */
  if (!h->used) munmap((void*)h,HUNK_MSIZE);
}

/* BLOCK MANAGER */

typedef struct Block_s Block_t;

struct Block_s /* 32-bytes long control structure (if 4-byte aligned) */
{
  char *ptr;                        /* pointer to related data */
  Block_t *next;                    /* next in free_mem list */
  Block_t *l_free_mem, *r_free_mem; /* left & right subtrees of <free_mem> */
  Block_t *l_ptrs, *r_ptrs;         /* left & right subtrees of <ptrs> */
  size_t size;                      /* size - divided by align */

  /* packed 4-byte attributes */
/* { */
  signed char bal_free_mem : 8;   /* balance of <free_mem> subtree */
  signed char bal_ptrs : 8;       /* balance of <ptrs> subtree */
  unsigned int used : 1;   /* used/free state of the block */
  unsigned int broken : 1; /* 1 if previous block can't be merged with it */
/* } */
};

static Block_t *bl_last; /* last mmapped block */

#define bl_get() hunk_alloc(sizeof(Block_t))
#define bl_rel(p) hunk_free((char*)p)

/* like C++ templates ;-) */

#include "avlmacro.h"

#define FREE_MEM_COMPARE(i,a,b) { i = (a)->size - (b)->size; }
#define PTRS_COMPARE(i,a,b) { i = (a)->ptr - (b)->ptr; }

Avl_Tree(free_mem,Block_t,free_mem,FREE_MEM_COMPARE)
Avl_Tree(ptrs,Block_t,ptrs,PTRS_COMPARE)

#define free_mem_root Avl_Root(Block_t, free_mem)
#define ptrs_root Avl_Root(Block_t, ptrs)

/* pp is freed block */
#define FREE_MEM_DEL_BLOCK(pp) \
{ \
  for (p = free_mem_root;;) \
    if (p->size > pp->size) p = p->l_free_mem; \
    else if (p->size < pp->size) p = p->r_free_mem; \
    else break; \
  if (p == pp) \
  { \
    if (pp->next) free_mem_replace(pp->next); \
    else free_mem_del(pp); \
  } \
  else \
  { \
    for (;p->next != pp; p = p->next); \
    p->next = pp->next; \
  } \
}

#define FREE_MEM_INS_BLOCK(pp) \
{ \
  if ((p = free_mem_ins(pp)) != NULL)\
  {\
    pp->next = p->next;\
    p->next = pp;\
  }\
  else pp->next = NULL; \
}

/* `b` is current block, `pp` is next block */
#define COMBINE_BLOCKS(b,pp) \
{\
  ptrs_del(pp); \
  b->size += pp->size; \
  if (pp == bl_last) bl_last = b; \
  bl_rel(pp); \
}

/* initializes new block b */
#define INIT_BLOCK(b, pppp, sz) \
{ \
  memset(b, 0, sizeof(Block_t)); \
  b->ptr  = pppp; \
  b->size = sz; \
  ptrs_ins(b); \
  FREE_MEM_INS_BLOCK(b); \
}

/* `b` is current block, `sz` its new size */
/* block `b` will be splitted to one busy & one free block */
#define SPLIT_BLOCK(b,sz) \
{\
  Block_t *bt; \
  bt = bl_get(); \
  INIT_BLOCK(bt, b->ptr + sz, b->size - sz); \
  b->size = sz; \
  if (bl_last == b) bl_last = bt; \
  bl_uncommit(bt);\
}

/* `b` is current block, `pp` is next free block, `sz` is needed size */
#define SHRINK_BLOCK(b,pp,sz) \
{\
  FREE_MEM_DEL_BLOCK(pp); \
  pp->ptr = b->ptr + sz; \
  pp->size += b->size - sz; \
  b->size = sz; \
  FREE_MEM_INS_BLOCK(pp); \
  bl_uncommit(pp); \
}

static Block_t *bl_mapnew(size_t size)
{
  size_t map_size;
  Block_t *pp, *p;
  void *pt;
  
  map_size = PAGE_ALIGN(size);
  pt = mmap(LARGE_MSTART,map_size,PROT_READ|PROT_WRITE|PROT_EXEC,
            MAP_PRIVATE|MAP_ANON,0,0);
  if (pt == MAP_FAILED) return (Block_t*)NULL;
  
  bl_last = pp = bl_get();
  INIT_BLOCK(pp, (char*)pt, map_size);
  pp->broken = 1;
  
  return pp;
}

static void bl_uncommit(Block_t *b)
{
  char *u_start, *u_end;
  
  u_start = PAGE_ALIGNP(b->ptr);
  u_end   = PAGE_DOWNALIGNP(b->ptr+b->size);
  if (u_end <= u_start) return;

#if M_DOTRIMMING
  mmap(u_start,u_end-u_start,PROT_READ|PROT_WRITE|PROT_EXEC,
    MAP_PRIVATE|MAP_ANON|MAP_FIXED,0,0);
#endif
}

/* requested size must be aligned to ALIGNMENT */
static Block_t *bl_alloc(size_t size)
{
  Block_t *p, *pp;
  
  /* try to find needed space in existing memory */
  for (p = free_mem_root, pp = NULL;p;)
  {
    if (p->size > size) { pp = p; p = p->l_free_mem; }
    else if (p->size < size) p = p->r_free_mem;
    else { pp = p; break; }
  }

  if (!pp)
  { /* map some memory */
    if (!bl_last)
    { /* just do initial mmap */
      pp = bl_mapnew(size);
      if (!pp) return NULL;
    }
    else if (!bl_last->used)
    { /* try growing last unused */
      if (mremap(PAGE_DOWNALIGNP(bl_last->ptr),
 PAGE_ALIGNP(bl_last->ptr+bl_last->size) - PAGE_DOWNALIGNP(bl_last->ptr),
 PAGE_ALIGNP(bl_last->ptr+size)-PAGE_DOWNALIGNP(bl_last->ptr),
        0) == MAP_FAILED)
      { /* unable to grow -- initiate new block */
	pp = bl_mapnew(size);
	if (!pp) return NULL;
      }
      else
      {
	pp = bl_last;
	FREE_MEM_DEL_BLOCK(pp);
	pp->size = PAGE_ALIGNP(pp->ptr+size) - pp->ptr;
	FREE_MEM_INS_BLOCK(pp);
      }
    }
    else
    { /* bl_last is used block */
      if (mremap(PAGE_DOWNALIGNP(bl_last->ptr),
PAGE_ALIGNP(bl_last->ptr+bl_last->size)-PAGE_DOWNALIGNP(bl_last->ptr),
PAGE_ALIGNP(bl_last->ptr+bl_last->size+size) - PAGE_DOWNALIGNP(bl_last->ptr),
	0) == MAP_FAILED)
      {
	pp = bl_mapnew(size);
	if (!pp) return NULL;
      }
      else
      {
	pp = bl_get();
	INIT_BLOCK(pp,bl_last->ptr+bl_last->size,
 PAGE_ALIGNP(bl_last->ptr+bl_last->size+size)-bl_last->ptr-bl_last->size);
	bl_last = pp;
      }
    }
  }
  
  /* just delete this node from free_mem tree */
  if (pp->next) free_mem_replace(pp->next); else free_mem_del(pp);
  pp->used = 1;
  
  if (pp->size - size > MALLOC_ALIGN)
  { /* this block can be splitted (it is unused,not_broken) */
    SPLIT_BLOCK(pp,size);
  }
  
  return pp;
}

static void bl_free(Block_t *b)
{
  Block_t *p, *bl_next, *bl_prev;
  
  /* Look for blocks before & after `b` */
  for (p = ptrs_root, bl_next = NULL, bl_prev = NULL; p;)
  {
    if      (p->ptr > b->ptr) { bl_next = p; p = p->l_ptrs; }
    else if (p->ptr < b->ptr) { bl_prev = p; p = p->r_ptrs; }
    else break;
  }
  if (b->l_ptrs)
    for (bl_prev = b->l_ptrs; bl_prev->r_ptrs; bl_prev = bl_prev->r_ptrs);
  if (b->r_ptrs)
    for (bl_next = b->r_ptrs; bl_next->l_ptrs; bl_next = bl_next->l_ptrs);
  
  if (bl_next && !bl_next->broken && !bl_next->used)
  {
    FREE_MEM_DEL_BLOCK(bl_next)
    COMBINE_BLOCKS(b,bl_next)
  }

  if (bl_prev && !b->broken && !bl_prev->used)
  {
    FREE_MEM_DEL_BLOCK(bl_prev)
    COMBINE_BLOCKS(bl_prev,b)
    b = bl_prev;
  }
  
  b->used = 0;
  FREE_MEM_INS_BLOCK(b)
  bl_uncommit(b);
}

static void malloc_init(void)
{
  int i, mapsize, x, old_x, gcount;
  
  mapsize = M_PAGESIZE;
  
  mmalloc_initialized = 0;
  bl_last = NULL;
  free_mem_root = NULL;
  ptrs_root = NULL;
  mapsize -= sizeof(Hunk_t);
  for (i = 1; i <= HUNK_MAXSIZE; i++)
  {
    free_h[i] = (Hunk_t*)NULL;
    for (x = mapsize/i, gcount = 0, old_x = 0; old_x != x;)
    {
      old_x = x;
      x = (mapsize - ALIGN(DIV8(old_x+7)))/i;
      if (gcount > 1 && x*i + ALIGN(DIV8(x+7)) <= mapsize) break;
      if (x*i + ALIGN(DIV8(x+7)) > mapsize) gcount++;
    }
    total_h[i] = x;
  }
  mutex_init(&malloc_lock);
  mmalloc_initialized = 1;
}

static void *mmalloc(size_t size)
{
  void *p;
  
  if (size == 0) return NULL;
 
  if (mmalloc_initialized < 0) malloc_init();
  if (mmalloc_initialized) mutex_lock(&malloc_lock);
  
  if (size <= HUNK_MAXSIZE)
    p = hunk_alloc(size);
  else
  {
    if ((p = bl_alloc(ALIGN(size))) != NULL)
      p = ((Block_t*)p)->ptr;
  }
  
  if (mmalloc_initialized) mutex_unlock(&malloc_lock);
  
  return p;
}

static void mfree(void *ptr)
{
  Block_t *p, *best;
 
  if (mmalloc_initialized < 0) return;
  if (mmalloc_initialized) mutex_lock(&malloc_lock);
  
  for (p = ptrs_root, best = NULL;p;)
  {
    if (p->ptr > (char*)ptr) p = p->l_ptrs;
    else { best = p; p = p->r_ptrs; }
  }
  
  if (!best || !best->used || best->ptr != (char*)ptr)
  {
    hunk_free(ptr);
    if (mmalloc_initialized) mutex_unlock(&malloc_lock);
    return;
  }
  
  bl_free(best);

  if (mmalloc_initialized) mutex_unlock(&malloc_lock);
}

static void *mrealloc_no_move(void *ptr, size_t size)
{
  Block_t *p, *best, *next;
  
  if (size <= HUNK_MAXSIZE) return NULL;
  
  if (mmalloc_initialized <= 0) return mmalloc(size);
  
  mutex_lock(&malloc_lock);
  
  /* Locate block */
  for (p = ptrs_root, best = NULL;p;)
  {
    if (p->ptr > (char*)ptr) p = p->l_ptrs;
    else { best = p; p = p->r_ptrs; }
  }
  
  if (!best || !best->used || best->ptr != (char*)ptr)
  {
    mutex_unlock(&malloc_lock);
    return NULL;
  }
  
  size = ALIGN(size);
  
  if (size == best->size)
  {
    mutex_unlock(&malloc_lock);
    return ptr;
  }
  
  if (best->r_ptrs) /* get block just after */
    for (next = best->r_ptrs; next->l_ptrs; next = next->l_ptrs);
  else
    for (p = ptrs_root, next = NULL;p;)
    {
      if (p->ptr > best->ptr) { next = p; p = p->l_ptrs; }
      else if (p->ptr < best->ptr) p = p->r_ptrs;
      else break;
    }
  
  if (size < best->size)
  { /* shrink block */
    if (!next || next->used || next->broken)
    {
      if (best->size - size > MALLOC_ALIGN)
      { /* do split */
	SPLIT_BLOCK(best,size);
      }
    }
    else
    { /* just move border of next block */
      SHRINK_BLOCK(best,next,size);
    }
  }
  else if (next && !next->broken && !next->used)
  { /* can expand */
    if (best->size + next->size > size + HUNK_MAXSIZE)
    { /* shrink next free block */
      SHRINK_BLOCK(best,next,size);
    }
    else if (best->size + next->size >= size)
    { /* combine blocks (eat next one) */
      FREE_MEM_DEL_BLOCK(next);
      COMBINE_BLOCKS(best,next);
    }
    else
    { /* not enough memory in next block */
      mutex_unlock(&malloc_lock);
      return NULL;
    }
  }
  else
  { /* no next block */
    mutex_unlock(&malloc_lock);
    return NULL;
  }
  mutex_unlock(&malloc_lock);
  return best->ptr;
}

static void *mrealloc(void *ptr, size_t size)
{
  void *tmp;
  
  tmp = mrealloc_no_move(ptr, size);
  
  if (!tmp)
  {
    Block_t *p, *best;
    
    mutex_lock(&malloc_lock);

    for (p = ptrs_root, best = NULL;p;)
    {
      if (p->ptr > (char*)ptr) p = p->l_ptrs;
      else { best = p; p = p->r_ptrs; }
    }
    
    if (!best || !best->used || best->ptr != (char*)ptr)
    {
      if (ptr)
      {
	Hunk_t *h;
        h = (Hunk_t*)PAGE_DOWNALIGNP(ptr);
        if (h->id == HUNK_ID)
        {
	  mutex_unlock(&malloc_lock);
	  if ((size >= HUNK_THRESHOLD && ALIGN(size) == h->size) ||
	      size == h->size) return ptr;
	  if ((tmp = mmalloc(size)) == NULL) return NULL;
	  mutex_lock(&malloc_lock);
	  memcpy(tmp,ptr,((size<h->size)?size:h->size));
	  hunk_free(ptr);
	  mutex_unlock(&malloc_lock);
	  return tmp;
	}
      }
      mutex_unlock(&malloc_lock);
      return mmalloc(size);
    }
    
    mutex_unlock(&malloc_lock);
    
    /* copy whole block */
    if ((tmp = mmalloc(size)) == NULL) return NULL;
    memcpy(tmp,ptr,((size<best->size)?size:best->size));
    
    mutex_lock(&malloc_lock);
    bl_free(best);
    mutex_unlock(&malloc_lock);
  }
  return tmp;
}

static void *mcalloc(size_t unit, size_t quantity)
{
  void *p;
  
  unit *= quantity;
 
  if ((p = mmalloc(unit)) == NULL) return NULL;
  memset(p,0,unit);
  return p;
}

/* PUBLIC functions */

void *malloc(size_t size) {
  return mmalloc(size);
}

void *calloc(size_t unit, size_t quantity) {
  return mcalloc(unit,quantity);
}

void *realloc(void *ptr, size_t size) {
  return mrealloc(ptr,size);
}

void free(void *ptr) {
  return mfree(ptr);
}


