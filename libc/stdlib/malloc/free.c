/*
 * libc/stdlib/malloc/free.c -- free function
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
#include <unistd.h>
#include <sys/mman.h>

#include "malloc.h"
#include "heap.h"


/* Try to release the free-area FA in HEAP back to the system.  */
static void
unmap_free_area (struct heap *heap, struct heap_free_area *fa)
{
  unsigned long start, end;
#ifndef MALLOC_USE_SBRK
  unsigned long unmap_start, unmap_end;
#endif

  end = (unsigned long)HEAP_FREE_AREA_END (fa);

#ifdef MALLOC_USE_SBRK
  /* When using sbrk, we only shrink the heap from the end.  It would be
     possible to allow _both_ -- shrinking via sbrk when possible, and
     otherwise shrinking via munmap, but this results in holes in memory
     that prevent the brk from every growing back down; since we only ever
     grow the heap via sbrk, this tends to produce a continuously growing
     brk (though the actual memory is unmapped), which could eventually run
     out of address space.  Note that `sbrk(0)' shouldn't normally do a
     system call, so this test is reasonably cheap.  */
  if ((void *)end != sbrk (0))
    {
      MALLOC_DEBUG ("  not unmapping: 0x%lx - 0x%lx (%d bytes)\n",
		    (unsigned long)HEAP_FREE_AREA_START (fa),
		    (unsigned long)HEAP_FREE_AREA_END (fa),
		    fa->size);
      return;
    }
#endif

  start = (unsigned long)HEAP_FREE_AREA_START (fa);

  MALLOC_DEBUG ("  unmapping: 0x%lx - 0x%lx (%ld bytes)\n",
		start, end, end - start);

  /* Remove FA from the heap.  */
  __heap_unlink_free_area (heap, fa);

  if (!fa->next && !fa->prev)
    /* We want to avoid the heap from losing all memory, so reserve a bit.
       This test is only a heuristic -- the existance of another free area,
       even if it's smaller than MALLOC_MIN_SIZE, will cause us not to
       reserve anything.  */
    {
      /* Put the reserved memory back in the heap; we asssume that
	 MALLOC_UNMAP_THRESHOLD is greater than MALLOC_MIN_SIZE, so we use
	 the latter unconditionally here.  */
      __heap_free (heap, (void *)start, MALLOC_MIN_SIZE);
      start += MALLOC_MIN_SIZE;
    }

#ifdef MALLOC_USE_SBRK

  sbrk (start - end);

#else /* !MALLOC_USE_SBRK */

  /* MEM/LEN may not be page-aligned, so we have to page-align them, and
     return any left-over bits on the end to the heap.  */
  unmap_start = MALLOC_ROUND_UP_TO_PAGE_SIZE (start);
  unmap_end = MALLOC_ROUND_DOWN_TO_PAGE_SIZE (end);

  /* We have to be careful that any left-over bits are large enough to
     return.  Note that we _don't check_ to make sure there's room to
     grow/shrink the start/end by another page, we just assume that the
     unmap threshold is high enough so that this is always safe (i.e., it
     should probably be at least 3 pages).  */
  if (unmap_start > start)
    {
      if (unmap_start - start < HEAP_MIN_FREE_AREA_SIZE)
	unmap_start += MALLOC_PAGE_SIZE;
      __heap_free (heap, (void *)start, unmap_start - start);
    }
  if (end > unmap_end)
    {
      if (end - unmap_end < HEAP_MIN_FREE_AREA_SIZE)
	unmap_end -= MALLOC_PAGE_SIZE;
      __heap_free (heap, (void *)unmap_end, end - unmap_end);
    }

  if (unmap_end > unmap_start)
    munmap ((void *)unmap_start, unmap_end - unmap_start);

#endif /* MALLOC_USE_SBRK */
}


void free (void *mem)
{
  if (mem)
    {
      size_t size;
      struct heap_free_area *fa;

      mem -= MALLOC_ALIGNMENT;
      size = *(size_t *)mem;

      MALLOC_DEBUG ("free: 0x%lx (base = 0x%lx, total_size = %d)\n",
		    (long)mem + MALLOC_ALIGNMENT, (long)mem, size);

      fa = __heap_free (&__malloc_heap, mem, size);

      /* Now we check to see if FA has grown big enough that it should be
	 unmapped.  */
      if (HEAP_FREE_AREA_SIZE (fa) >= MALLOC_UNMAP_THRESHOLD)
	/* Get rid of it.  */
	unmap_free_area (&__malloc_heap, fa);
    }
}
