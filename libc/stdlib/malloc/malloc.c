/*
 * libc/stdlib/malloc/malloc.c -- malloc function
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
#include <sys/mman.h>

#include "malloc.h"
#include "heap.h"


/* When we give memory to the heap, start this many bytes after the
   beginning of the mmaped block.  This is because we must ensure that
   malloc return values are aligned to MALLOC_ALIGNMENT, but since we need
   to use one word _before_ the beginning of that, we actually want the heap
   to return values that are MALLOC_ALIGNMENT aligned - sizeof (size_t).
   Since the heap always allocates in multiples of HEAP_GRANULARITY, we can
   do this by (1) ensuring that HEAP_GRANULARITY is a multiple of
   MALLOC_ALIGNMENT, and (2) making sure that the heap's free areas start
   sizeof(size_t) bytes before our required alignment.  */
#define MALLOC_HEAP_BLOCK_SHIM	(MALLOC_ALIGNMENT - sizeof (size_t))


/* The heap used for small allocations.  */
struct heap __malloc_heap = HEAP_INIT;


void *malloc (size_t size)
{
  void *mem;

  MALLOC_DEBUG ("malloc: %d bytes\n", size);

  /* Include an extra word to record the size of the allocated block.  */
  size += sizeof (size_t);

  if (size >= MALLOC_MMAP_THRESHOLD)
    /* Use mmap for large allocations.  */
    {
      /* Make sure we request enough memory to align the result correctly,
	 and that SIZE reflects that mmap hands back whole pages.  */
      size += MALLOC_ROUND_UP_TO_PAGE_SIZE (MALLOC_ALIGNMENT - sizeof(size_t));

      mem = mmap (0, size, PROT_READ | PROT_WRITE,
		  MAP_SHARED | MAP_ANONYMOUS, 0, 0);
      if (mem == MAP_FAILED)
	return 0;
    }
  else
    /* Use the heap for small allocations.  */
    {
      mem = __heap_alloc (&__malloc_heap, &size);

      if (! mem) 
	/* We couldn't allocate from the heap, so get some more memory
	   from the system, add it to the heap, and try again.  */
	{
	  /* If we're trying to allocate a block bigger than the default
	     MALLOC_HEAP_EXTEND_SIZE, make sure we get enough to hold it. */
	  size_t block_size = (size < MALLOC_HEAP_EXTEND_SIZE
			       ? MALLOC_HEAP_EXTEND_SIZE
			       : MALLOC_ROUND_UP_TO_PAGE_SIZE (size));
	  /* Allocate the new heap block.  */
	  void *block = mmap (0, block_size,
			      PROT_READ | PROT_WRITE,
			      MAP_SHARED | MAP_ANONYMOUS, 0, 0);

	  if (block != MAP_FAILED) 
	    {
	      /* Put BLOCK into the heap.  We first try to append BLOCK to
		 an existing free area, which is more efficient because it
		 doesn't require using a `shim' at the beginning (which
		 would prevent merging free-areas); since mmap often returns
		 contiguous areas, this is worth it.  */
	      if (! __heap_append_free (&__malloc_heap, block, block_size))
		/* Couldn't append, just add BLOCK as a new free-area.  */
		__heap_free (&__malloc_heap,
			     block + MALLOC_HEAP_BLOCK_SHIM,
			     block_size - MALLOC_HEAP_BLOCK_SHIM);

	      /* Try again to allocate.  */
	      mem = __heap_alloc (&__malloc_heap, &size);
	    }
	}
    }

  if (mem)
    /* Record the size of this block just before the returned address.  */
    {
      *(size_t *)mem = size;
      mem = (size_t *)mem + 1;

      MALLOC_DEBUG ("  malloc: returning 0x%lx (base:0x%lx, total_size:%d)\n",
		    (long)mem, (long)mem - sizeof (size_t), size);
    }

  return mem;
}
