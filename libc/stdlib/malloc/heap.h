/*
 * libc/stdlib/malloc/heap.h -- heap allocator used for malloc
 *
 *  Copyright (C) 2002  NEC Corporation
 *  Copyright (C) 2002  Miles Bader <miles@gnu.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License.  See the file COPYING.LIB in the main
 * directory of this archive for more details.
 * 
 * Written by Miles Bader <miles@gnu.org>
 */

#include <features.h>


#ifdef __UCLIBC_HAS_THREADS__
#include <pthread.h>
typedef pthread_mutex_t heap_mutex_t;
# define HEAP_MUTEX_INIT	PTHREAD_MUTEX_INITIALIZER
# define __heap_lock(heap)	pthread_mutex_lock (&(heap)->lock)
# define __heap_unlock(heap)	pthread_mutex_unlock (&(heap)->lock);
#else
/* Without threads, Mutex operations are a nop.  */
typedef int heap_mutex_t;
# define HEAP_MUTEX_INIT	0
# define __heap_lock(heap)
# define __heap_unlock(heap)
#endif



/* The unit in which allocation is done, due to alignment constraints, etc.
   All allocation requests are rounded up to a multiple of this size.
   Must be a power of 2.  */
#define HEAP_GRANULARITY	(sizeof (double))


/* A heap is a collection of memory blocks, from which smaller blocks
   of memory can be allocated.  */
struct heap
{
  struct heap_free_area *free_areas;
  heap_mutex_t lock;
};

#define HEAP_INIT 	{ 0, HEAP_MUTEX_INIT }


/* A free-list area `header'.  These are actually stored at the _ends_ of
   free areas (to make allocating from the beginning of the area simpler),
   so one might call it a `footer'.  */
struct heap_free_area
{
	size_t size;
	struct heap_free_area *next, *prev;
};

/* Return the address of the end of the frea area FA.  */
#define HEAP_FREE_AREA_END(fa) ((void *)(fa + 1))
/* Return the address of the beginning of the frea area FA.  FA is
   evaulated multiple times.  */
#define HEAP_FREE_AREA_START(fa) ((void *)((char *)(fa + 1) - (fa)->size))


/* Rounds SZ up to be a multiple of HEAP_GRANULARITY.  */
#define HEAP_ADJUST_SIZE(sz)  \
   (((sz) + HEAP_GRANULARITY - 1) & ~(HEAP_GRANULARITY - 1))

/* The minimum size of a free area.  It must include at least enough room
   to hold a struct heap_free_area, plus enough extra to be usefully
   allocated.  */
#define HEAP_MIN_FREE_AREA_SIZE  \
  (sizeof (struct heap_free_area) + HEAP_ADJUST_SIZE (1))


#if 0
#include <stdio.h>
static void HEAP_DEBUG (struct heap *heap, const char *str)
{
  static int recursed = 0;
  if (! recursed)
    {
      struct heap_free_area *fa;
      recursed = 1;
      fprintf (stderr, "  %s: heap @0x%lx:\n", str, (long)heap);
      for (fa = heap->free_areas; fa; fa = fa->next)
	fprintf (stderr,
		 "    0x%lx:  0x%lx - 0x%lx  (%d)\tN=0x%lx, P=0x%lx\n",
		 (long)fa,
		 (long)HEAP_FREE_AREA_START (fa),
		 (long)HEAP_FREE_AREA_END (fa),
		 fa->size,
		 (long)fa->prev,
		 (long)fa->next);
      recursed = 0;
    }
}
#else
#define HEAP_DEBUG(heap, str) (void)0
#endif


/* Allocate SIZE bytes from the front of the free-area FA in HEAP, and
   return the amount actually allocated (which may be more than SIZE).  */
extern inline size_t
__heap_free_area_alloc (struct heap *heap,
			struct heap_free_area *fa, size_t size)
{
  size_t fa_size = fa->size;

  if (fa_size < size + HEAP_MIN_FREE_AREA_SIZE)
    /* There's not enough room left over in FA after allocating the block, so
       just use the whole thing, removing it from the list of free areas.  */
    {
      if (fa->next)
	fa->next->prev = fa->prev;
      if (fa->prev)
	fa->prev->next = fa->next;
      else
	heap->free_areas = fa->next;
      /* Remember that we've alloced the whole area.  */
      size = fa_size;
    }
  else
    /* Reduce size of FA to account for this allocation.  */
    fa->size = fa_size - size;

  return size;
}


/* Allocate and return a block at least *SIZE bytes long from HEAP.
   *SIZE is adjusted to reflect the actual amount allocated (which may be
   greater than requested).  */
extern void *__heap_alloc (struct heap *heap, size_t *size);

/* Allocate SIZE bytes at address MEM in HEAP.  Return the actual size
   allocated, or 0 if we failed.  */
extern size_t __heap_alloc_at (struct heap *heap, void *mem, size_t size);

/* Return the memory area MEM of size SIZE to HEAP.  */
extern void __heap_free (struct heap *heap, void *mem, size_t size);

/* If the memory area MEM, of size SIZE, immediately follows an existing
   free-area in HEAP, use it to extend that free-area, and return true;
   otherwise return false.  */
extern int __heap_append_free (struct heap *heap, void *mem, size_t size);
