/*
 * libc/stdlib/malloc/malloc.h -- small malloc implementation
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

/* The alignment we guarantee for malloc return values.  */
#define MALLOC_ALIGNMENT	(sizeof (double))

/* The system pagesize we assume; we really ought to get it with
   getpagesize, but gee, how annoying.  */
#define MALLOC_PAGE_SIZE	4096

/* The minimum size of block we request from the the system to extend the
   heap for small allocations (we may request a bigger block if necessary to
   satisfy a particularly big request).  */
#define MALLOC_HEAP_EXTEND_SIZE	MALLOC_PAGE_SIZE

/* When a heap free-area grows above this size, try to unmap it, releasing
   the memory back to the system.  */
#define MALLOC_UNMAP_THRESHOLD	(8*MALLOC_PAGE_SIZE)
/* When unmapping a free-area, retain this many bytes if it's the only one,
   to avoid completely emptying the heap.  This is only a heuristic -- the
   existance of another free area, even if it's smaller than
   MALLOC_MIN_SIZE, will cause us not to reserve anything.  */
#define MALLOC_MIN_SIZE		(2*MALLOC_PAGE_SIZE)


/* For systems with an MMU, use sbrk to map/unmap memory for the malloc
   heap, instead of mmap/munmap.  This is a tradeoff -- sbrk is faster than
   mmap/munmap, and guarantees contiguous allocation, but is also less
   flexible, and causes the heap to only be shrinkable from the end.  */
#ifdef __UCLIBC_HAS_MMU__
#define MALLOC_USE_SBRK
#endif


/* Change this to `#if 1' to cause malloc to emit debugging info to stderr.  */
#if 0
#include <stdio.h>
#define MALLOC_DEBUG(fmt, args...) fprintf (stderr, fmt , ##args)
#else
#define MALLOC_DEBUG(fmt, args...) (void)0
#endif


/* Return SZ rounded down to POWER_OF_2_SIZE (which must be power of 2).  */
#define MALLOC_ROUND_DOWN(sz, power_of_2_size)  \
  ((sz) & ~(power_of_2_size - 1))
/* Return SZ rounded to POWER_OF_2_SIZE (which must be power of 2).  */
#define MALLOC_ROUND_UP(sz, power_of_2_size)				\
  MALLOC_ROUND_DOWN ((sz) + (power_of_2_size - 1), (power_of_2_size))

/* Return SZ rounded down to a multiple MALLOC_PAGE_SIZE.  */
#define MALLOC_ROUND_DOWN_TO_PAGE_SIZE(sz)  \
  MALLOC_ROUND_DOWN (sz, MALLOC_PAGE_SIZE)
/* Return SZ rounded up to a multiple MALLOC_PAGE_SIZE.  */
#define MALLOC_ROUND_UP_TO_PAGE_SIZE(sz)  \
  MALLOC_ROUND_UP (sz, MALLOC_PAGE_SIZE)


/* The malloc heap.  */
extern struct heap __malloc_heap;
