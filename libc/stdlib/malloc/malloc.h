/*
 * libc/stdlib/malloc-zarg/malloc.h -- small malloc implementation
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

/* The threshold above which blocks are allocated/freed with mmap/munmap,
   rather than using the heap.  */
#define MALLOC_MMAP_THRESHOLD	(8*MALLOC_PAGE_SIZE)


#if 0
#include <stdio.h>
#define MALLOC_DEBUG(fmt, args...) fprintf (stderr, fmt , ##args)
#else
#define MALLOC_DEBUG(fmt, args...) (void)0
#endif


/* Return SZ rounded up to a multiple MALLOC_PAGE_SIZE.  */
#define MALLOC_ROUND_UP_TO_PAGE_SIZE(sz)  \
  (((sz) + (MALLOC_PAGE_SIZE - 1)) & ~(MALLOC_PAGE_SIZE - 1))


/* The heap used for small allocations.  */
extern struct heap __malloc_heap;
