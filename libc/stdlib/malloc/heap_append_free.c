/*
 * libc/stdlib/malloc-zarg/heap_append_free.c -- append to heap free area
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


/* If the block MEM, of size SIZE, immediately follows an existing free-area
   in HEAP, use it to extend that free-area, and return true; otherwise return
   false.  */
int
__heap_append_free (struct heap *heap, void *mem, size_t size)
{
  int success = 0;
  struct heap_free_area *fa;

  mutex_lock (heap->lock);

  HEAP_DEBUG (heap, "before __heap_append_free");

  /* Find an adjacent free-list entry.  */
  for (fa = heap->free_areas; fa; fa = fa->next)
    if (HEAP_FREE_AREA_END (fa) == mem)
      /* MEM follows FA, extend FA to include it.  Since the descriptor for FA
	 is located at the end, we must actually write a new descriptor.  Note
	 that we _don't_ handle the case where the extended FA can be merged
	 with a following free area; this is because this function is
	 generally only used in cases were we believe that usually won't
	 happen (it doesn't cause any incorrectness, and the two blocks can be
	 merged by __heap_free later).  */
      {
	struct heap_free_area *next_fa = fa->next;
	struct heap_free_area *prev_fa = fa->prev;
	size_t fa_size = fa->size;
	struct heap_free_area *new_fa =
	  (struct heap_free_area *)((char *)fa + size);

	/* Update surrounding free-areas to point to FA's new address.  */
	if (prev_fa)
	  prev_fa->next = new_fa;
	else
	  heap->free_areas = new_fa;
	if (next_fa)
	  next_fa->prev = new_fa;

	/* Fill in the moved descriptor.  */
	new_fa->prev = prev_fa;
	new_fa->next = next_fa;
	new_fa->size = fa_size + size;

	success = 1;
	break;
      }

  HEAP_DEBUG (heap, "after __heap_append_free");

  mutex_unlock (heap->lock);

  return success;
}
