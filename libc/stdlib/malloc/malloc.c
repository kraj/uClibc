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

#ifdef MALLOC_USE_LOCKING
/* A lock protecting the malloc heap.  */
malloc_mutex_t __malloc_lock;
# ifdef MALLOC_USE_SBRK
/* A lock protecting our use of sbrk.  */
malloc_mutex_t __malloc_sbrk_lock;
# endif /* MALLOC_USE_SBRK */
#endif /* MALLOC_USE_LOCKING */

#ifdef MALLOC_DEBUGGING
int __malloc_debug = 0;
#endif


void *
malloc (size_t size)
{
  void *mem;
  struct heap *heap = &__malloc_heap;

  MALLOC_DEBUG ("malloc: %d bytes\n", size);

  /* Include extra space to record the size of the allocated block.  */
  size += MALLOC_HEADER_SIZE;

  __malloc_lock ();

  /* First try to get memory that's already in our heap.  */
  mem = __heap_alloc (heap, &size);

  if (unlikely (! mem))
    /* We couldn't allocate from the heap, so grab some more
       from the system, add it to the heap, and try again.  */
    {
      /* If we're trying to allocate a block bigger than the default
	 MALLOC_HEAP_EXTEND_SIZE, make sure we get enough to hold it. */
      void *block;
      size_t block_size
	= (size < MALLOC_HEAP_EXTEND_SIZE
	   ? MALLOC_HEAP_EXTEND_SIZE
	   : MALLOC_ROUND_UP_TO_PAGE_SIZE (size));

#ifdef MALLOC_USE_SBRK
      /* Get the sbrk lock while we've still got the main lock.  */
      __malloc_lock_sbrk ();
#endif

      /* Don't hold the main lock during the syscall, so that small
	 allocations in a different thread may succeed while we're
	 blocked.  */
      __malloc_unlock ();

      /* Allocate the new heap block.  */
#ifdef MALLOC_USE_SBRK

      /* Use sbrk we can, as it's faster than mmap, and guarantees
	 contiguous allocation.  */
      block = sbrk (block_size);
      if (likely (block != (void *)-1))
	{
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
	}
      __malloc_unlock_sbrk ();

#else /* !MALLOC_USE_SBRK */

      /* Otherwise, use mmap.  */
      block = mmap (0, block_size, PROT_READ | PROT_WRITE,
		    MAP_SHARED | MAP_ANONYMOUS, 0, 0);

#endif /* MALLOC_USE_SBRK */

      /* Get back the main lock.  */
      __malloc_lock ();

      if (likely (block != (void *)-1))
	{
	  MALLOC_DEBUG ("  adding memory: 0x%lx - 0x%lx (%d bytes)\n",
			(long)block, (long)block + block_size, block_size);

	  /* Put BLOCK into the heap.  */
	  __heap_free (heap, block, block_size);

	  /* Try again to allocate.  */
	  mem = __heap_alloc (heap, &size);
	}
    }

  __malloc_unlock ();

  if (likely (mem))
    /* Record the size of the block and get the user address.  */
    {
      mem = MALLOC_SETUP (mem, size);

      MALLOC_DEBUG ("  malloc: returning 0x%lx (base:0x%lx, total_size:%d)\n",
		    (long)mem, (long)MALLOC_BASE(mem), (long)MALLOC_SIZE(mem));
    }

  return mem;
}
