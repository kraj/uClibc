/*
 * libc/stdlib/malloc-zarg/heap_free.c -- return memory to a heap
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

#include <stdlib.h>

#include "heap.h"


/* Return the memory area MEM of size SIZE to HEAP.  */
void
__heap_free (struct heap *heap, void *mem, size_t size)
{
  struct heap_free_area *prev_fa, *fa, *new_fa;
  void *end = (char *)mem + size;

  mutex_lock (heap->lock);

  HEAP_DEBUG (heap, "before __heap_free");

  /* Find an adjacent free-list entry.  */
  for (prev_fa = 0, fa = heap->free_areas; fa; prev_fa = fa, fa = fa->next)
    {
      size_t fa_size = fa->size;
      void *fa_end = HEAP_FREE_AREA_END (fa);
      void *fa_mem = HEAP_FREE_AREA_START (fa);

      if (fa_mem == end)
	/* FA is just after MEM, grow down to encompass it. */
	{
	  fa_size += size;

	  /* See if FA can now be merged with its predecessor. */
	  if (prev_fa && fa_mem - size == HEAP_FREE_AREA_END (prev_fa))
	    /* Yup; merge PREV_FA's info into FA.  */
	    {
	      struct heap_free_area *pp = prev_fa->prev;
	      fa_size += prev_fa->size;
	      if (pp)
		pp->next = fa;
	      else
		heap->free_areas = fa;
	      fa->prev = pp;
	    }

	  fa->size = fa_size;

	  goto done;
	}
      else if (fa_end == mem)
	/* FA is just before MEM, expand to encompass it. */
	{
	  struct heap_free_area *next_fa = fa->next;

	  fa_size += size;

	  /* See if FA can now be merged with its successor. */
	  if (next_fa && fa_end + size == HEAP_FREE_AREA_START (next_fa))
	    {
	      /* Yup; merge FA's info into NEXT_FA.  */
	      fa_size += next_fa->size;
	      if (prev_fa)
		prev_fa->next = next_fa;
	      else
		heap->free_areas = next_fa;
	      next_fa->prev = prev_fa;
	      fa = next_fa;
	    }
	  else
	    /* FA can't be merged; move the descriptor for it to the tail-end
	       of the memory block.  */
	    {
	      new_fa = (struct heap_free_area *)((char *)fa + size);
	      /* Update surrounding free-areas to point to FA's new address. */
	      if (prev_fa)
		prev_fa->next = new_fa;
	      else
		heap->free_areas = new_fa;
	      if (next_fa)
		next_fa->prev = new_fa;
	      /* Fill in the moved descriptor.  */
	      new_fa->prev = prev_fa;
	      new_fa->next = next_fa;
	      fa = new_fa;
	    }

	  fa->size = fa_size;

	  goto done;
	}
      else if (fa_mem > mem)
	/* We've reached the right spot in the free-list without finding an
	   adjacent free-area, so add a new free area to hold MEM. */
	break;
    }

  /* Make a new free-list entry.  */

  /* NEW_FA initially holds only MEM.  */
  new_fa = (struct heap_free_area *)
    ((char *)mem + size - sizeof (struct heap_free_area));
  new_fa->size = size;
  new_fa->next = fa;
  new_fa->prev = prev_fa;

  /* Insert NEW_FA in the free-list between PREV_FA and FA. */
  if (prev_fa)
    prev_fa->next = new_fa;
  else
    heap->free_areas = new_fa;
  if (fa)
    fa->prev = new_fa;

 done:
  HEAP_DEBUG (heap, "after __heap_free");

  mutex_unlock (heap->lock);
}
