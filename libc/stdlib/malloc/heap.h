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


/* The heap allocates in multiples of, and aligned to, HEAP_GRANULARITY.
   HEAP_GRANULARITY must be a power of 2.  Malloc depends on this being the
   same as MALLOC_ALIGNMENT.  */
#define HEAP_GRANULARITY	(sizeof (double))


/* A heap is a collection of memory blocks, from which smaller blocks
   of memory can be allocated.  */
struct heap
{
  /* A list of memory in the heap available for allocation.  */
  struct heap_free_area *free_areas;
};
#define HEAP_INIT 	{ 0 }


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
/* Return the size of the frea area FA.  */
#define HEAP_FREE_AREA_SIZE(fa) ((fa)->size)


/* Rounds SZ up to be a multiple of HEAP_GRANULARITY.  */
#define HEAP_ADJUST_SIZE(sz)  \
   (((sz) + HEAP_GRANULARITY - 1) & ~(HEAP_GRANULARITY - 1))


/* The minimum allocatable size.  */
#define HEAP_MIN_SIZE	HEAP_ADJUST_SIZE (sizeof (struct heap_free_area))

/* The minimum size of a free area; if allocating memory from a free-area
   would make the free-area smaller than this, the allocation is simply
   given the whole free-area instead.  It must include at least enough room
   to hold a struct heap_free_area, plus some extra to avoid excessive heap
   fragmentation (thus increasing speed).  This is only a heuristic -- it's
   possible for smaller free-areas than this to exist (say, by realloc
   returning the tail-end of a previous allocation), but __heap_alloc will
   try to get rid of them when possible.  */
#define HEAP_MIN_FREE_AREA_SIZE  \
  HEAP_ADJUST_SIZE (sizeof (struct heap_free_area) + 32)


/* Define HEAP_DEBUGGING to cause the heap routines to emit debugging info
   to stderr.  */
#ifdef HEAP_DEBUGGING
#include <stdio.h>
static void HEAP_DEBUG (struct heap *heap, const char *str)
{
  static int recursed = 0;
  if (! recursed)
    {
      struct heap_free_area *fa, *prev;
      recursed = 1;
      fprintf (stderr, "  %s: heap @0x%lx:\n", str, (long)heap);
      for (prev = 0, fa = heap->free_areas; fa; prev = fa, fa = fa->next)
	{
	  fprintf (stderr,
		   "    0x%lx:  0x%lx - 0x%lx  (%d)\tP=0x%lx, N=0x%lx\n",
		   (long)fa,
		   (long)HEAP_FREE_AREA_START (fa),
		   (long)HEAP_FREE_AREA_END (fa),
		   fa->size,
		   (long)fa->prev,
		   (long)fa->next);
	  if (fa->prev != prev)
	    fprintf (stderr,
		     "      PREV POINTER CORRUPTED!!!!  P=0x%lx should be 0x%lx\n",
		     (long)fa->prev, (long)prev);
	}
      recursed = 0;
    }
}
#else
#define HEAP_DEBUG(heap, str) (void)0
#endif


/* Remove the free-area FA from HEAP.  */
extern inline void
__heap_unlink_free_area (struct heap *heap, struct heap_free_area *fa)
{
  if (fa->next)
    fa->next->prev = fa->prev;
  if (fa->prev)
    fa->prev->next = fa->next;
  else
    heap->free_areas = fa->next;
}

/* Link the free-area FA between the existing free-area's PREV and NEXT in
   HEAP.  PREV and NEXT may be 0; if PREV is 0, FA is installed as the
   first free-area.  */
extern inline void
__heap_link_free_area (struct heap *heap, struct heap_free_area *fa,
		       struct heap_free_area *prev,
		       struct heap_free_area *next)
{
  fa->next = next;
  fa->prev = prev;

  if (prev)
    prev->next = fa;
  else
    heap->free_areas = fa;
  if (next)
    next->prev = fa;
}

/* Update the mutual links between the free-areas PREV and FA in HEAP.
   PREV may be 0, in which case FA is installed as the first free-area (but
   FA may not be 0).  */
extern inline void
__heap_link_free_area_after (struct heap *heap,
			     struct heap_free_area *fa,
			     struct heap_free_area *prev)
{
  if (prev)
    prev->next = fa;
  else
    heap->free_areas = fa;
  fa->prev = prev;
}

/* Add a new free-area MEM, of length SIZE, in between the existing
   free-area's PREV and NEXT in HEAP, and return a pointer to its header.
   PREV and NEXT may be 0; if PREV is 0, MEM is installed as the first
   free-area.  */
extern inline struct heap_free_area *
__heap_add_free_area (struct heap *heap, void *mem, size_t size,
		      struct heap_free_area *prev,
		      struct heap_free_area *next)
{
  struct heap_free_area *fa = (struct heap_free_area *)
    ((char *)mem + size - sizeof (struct heap_free_area));

  fa->size = size;

  __heap_link_free_area (heap, fa, prev, next);

  return fa;
}


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
      __heap_unlink_free_area (heap, fa);
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

/* Return the memory area MEM of size SIZE to HEAP.
   Returns the heap free area into which the memory was placed.  */
extern struct heap_free_area *__heap_free (struct heap *heap,
					   void *mem, size_t size);
