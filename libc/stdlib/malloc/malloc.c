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
#include <unistd.h>
#include <sys/mman.h>

#include "malloc.h"
#include "heap.h"


/* The malloc heap.  */
struct heap __malloc_heap = HEAP_INIT;


void *malloc (size_t size)
{
  void *mem;

  MALLOC_DEBUG ("malloc: %d bytes\n", size);

  /* Include extra space to record the size of the allocated block.  */
  size += MALLOC_ROUND_UP (sizeof (size_t), MALLOC_ALIGNMENT);

  mem = __heap_alloc (&__malloc_heap, &size);
  if (! mem) 
    /* We couldn't allocate from the heap, so get some more memory
       from the system, add it to the heap, and try again.  */
    {
      /* If we're trying to allocate a block bigger than the default
	 MALLOC_HEAP_EXTEND_SIZE, make sure we get enough to hold it. */
      size_t block_size
	= (size < MALLOC_HEAP_EXTEND_SIZE
	   ? MALLOC_HEAP_EXTEND_SIZE
	   : MALLOC_ROUND_UP_TO_PAGE_SIZE (size));
      /* Allocate the new heap block.  */
#ifdef MALLOC_USE_SBRK
      /* Use sbrk we can, as it's faster than mmap, and guarantees
	 contiguous allocation.  */
      void *block = sbrk (block_size);
#else
      /* Otherwise, use mmap.  */
      void *block = mmap (0, block_size, PROT_READ | PROT_WRITE,
			  MAP_SHARED | MAP_ANONYMOUS, 0, 0);
#endif

      if (block != (void *)-1)
	{
#ifdef MALLOC_USE_SBRK
	  /* Because sbrk can return results of arbitrary
	     alignment, align the result to a MALLOC_ALIGNMENT boundary.  */
	  long aligned_block = MALLOC_ROUND_UP ((long)block, MALLOC_ALIGNMENT);
	  if (block != (void *)aligned_block)
	    /* Have to adjust.  We should only have to actually do this
	       the first time (after which we will have aligned the brk
	       correctly).  */
	    {
	      /* Move the brk to reflect the alignment; our next allocation
		 should start on exactly the right alignment.  */
	      sbrk (aligned_block - (long)block);
	      block = (void *)aligned_block;
	    }
#endif /* MALLOC_USE_SBRK */

	  MALLOC_DEBUG ("  adding memory: 0x%lx - 0x%lx (%d bytes)\n",
			(long)block, (long)block + block_size, block_size);

	  /* Put BLOCK into the heap.  */
	  __heap_free (&__malloc_heap, block, block_size);

	  /* Try again to allocate.  */
	  mem = __heap_alloc (&__malloc_heap, &size);
	}
    }

  if (mem)
    /* Record the size of this block just before the returned address.  */
    {
      *(size_t *)mem = size;
      mem += MALLOC_ALIGNMENT;

      MALLOC_DEBUG ("  malloc: returning 0x%lx (base:0x%lx, total_size:%d)\n",
		    (long)mem, (long)mem - MALLOC_ALIGNMENT, size);
    }

  return mem;
}
