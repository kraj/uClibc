/*
 * libc/stdlib/malloc/heap_debug.c -- optional heap debugging routines
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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "heap.h"


#ifdef HEAP_DEBUGGING
int __heap_debug = 0;
#endif


static void
__heap_dump_freelist (struct heap *heap)
{
  struct heap_free_area *fa;
  for (fa = heap->free_areas; fa; fa = fa->next)
    fprintf (stderr,
	     "    0x%lx:  0x%lx - 0x%lx  (%d)\tP=0x%lx, N=0x%lx\n",
	     (long)fa,
	     (long)HEAP_FREE_AREA_START (fa),
	     (long)HEAP_FREE_AREA_END (fa),
	     fa->size,
	     (long)fa->prev,
	     (long)fa->next);
}

/* Output a text representation of HEAP to stderr, labelling it with STR.  */
void
__heap_dump (struct heap *heap, const char *str)
{
  static int recursed = 0;

  if (! recursed)
    {
      __heap_check (heap, str);

      recursed = 1;

      fprintf (stderr, "  %s: heap @0x%lx:\n", str, (long)heap);
      __heap_dump_freelist (heap);

      recursed = 0;
    }
}


/* Do some consistency checks on HEAP.  If they fail, output an error
   message to stderr, and exit.  STR is printed with the failure message.  */
void
__heap_check (struct heap *heap, const char *str)
{
  struct heap_free_area *fa, *prev;

  for (prev = 0, fa = heap->free_areas; fa; prev = fa, fa = fa->next)
    if (fa->prev != prev)
      {
	if (str)
	  fprintf (stderr, "\nHEAP CHECK FAILURE %s: ", str);
	else
	  fprintf (stderr, "\nHEAP CHECK FAILURE: ");

	fprintf (stderr,
		 " prev pointer corrupted:  P=0x%lx should be 0x%lx\n",
		 (long)fa->prev, (long)prev);
	fprintf (stderr, "\nheap:\n");

	__heap_dump_freelist (heap);

	exit (22);
      }
}
