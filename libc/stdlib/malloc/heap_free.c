/*
 * libc/stdlib/malloc/heap_free.c -- return memory to a heap
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
struct heap_free_area *
__heap_free (struct heap *heap, void *mem, size_t size)
{
  struct heap_free_area *prev_fa, *fa;
  void *end = (char *)mem + size;

  HEAP_DEBUG (heap, "before __heap_free");

  /* Find an adjacent free-list entry.  */
  for (prev_fa = 0, fa = heap->free_areas; fa; prev_fa = fa, fa = fa->next)
    {
      size_t fa_size = fa->size;
      void *fa_mem = HEAP_FREE_AREA_START (fa);

      if (end == fa_mem)
	/* FA is just after MEM, grow down to encompass it. */
	{
	  fa_size += size;

	  /* See if FA can now be merged with its predecessor. */
	  if (prev_fa && fa_mem - size == HEAP_FREE_AREA_END (prev_fa))
	    /* Yup; merge PREV_FA's info into FA.  */
	    {
	      fa_size += prev_fa->size;
	      __heap_link_free_area_after (heap, fa, prev_fa->prev);
	    }

	  fa->size = fa_size;

	  goto done;
	}
      else if (HEAP_FREE_AREA_END (fa) == mem)
	/* FA is just before MEM, expand to encompass it. */
	{
	  struct heap_free_area *next_fa = fa->next;

	  fa_size += size;

	  /* See if FA can now be merged with its successor. */
	  if (next_fa && mem + size == HEAP_FREE_AREA_START (next_fa))
	    /* Yup; merge FA's info into NEXT_FA.  */
	    {
	      fa_size += next_fa->size;
	      __heap_link_free_area_after (heap, next_fa, prev_fa);
	      fa = next_fa;
	    }
	  else
	    /* FA can't be merged; move the descriptor for it to the tail-end
	       of the memory block.  */
	    {
	      /* The new descriptor is at the end of the extended block,
		 SIZE bytes later than the old descriptor.  */
	      fa = (struct heap_free_area *)((char *)fa + size);
	      /* Update links with the neighbors in the list.  */ 
	      __heap_link_free_area (heap, fa, prev_fa, next_fa);
	    }

	  fa->size = fa_size;

	  goto done;
	}
      else if (fa_mem > mem)
	/* We've reached the right spot in the free-list without finding an
	   adjacent free-area, so continue below to add a new free area. */
	break;
    }

  /* Make MEM into a new free-list entry.  */
  fa = __heap_add_free_area (heap, mem, size, prev_fa, fa);

 done:
  HEAP_DEBUG (heap, "after __heap_free");

  return fa;
}
