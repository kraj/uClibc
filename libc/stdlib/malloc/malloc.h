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

/* When realloc shrinks an allocation, it only does so if more than this
   many bytes will be freed; it must at at least HEAP_MIN_SIZE.  Larger
   values increase speed (by reducing heap fragmentation) at the expense of
   space.  */
#define MALLOC_REALLOC_MIN_FREE_SIZE  (HEAP_MIN_SIZE + 16)


/* For systems with an MMU, use sbrk to map/unmap memory for the malloc
   heap, instead of mmap/munmap.  This is a tradeoff -- sbrk is faster than
   mmap/munmap, and guarantees contiguous allocation, but is also less
   flexible, and causes the heap to only be shrinkable from the end.  */
#ifdef __UCLIBC_HAS_MMU__
# define MALLOC_USE_SBRK
#endif


/* The size of a malloc allocation is stored in a size_t word
   MALLOC_ALIGNMENT bytes prior to the start address of the allocation:

     +--------+---------+-------------------+
     | SIZE   |(unused) | allocation  ...   |
     +--------+---------+-------------------+
     ^ BASE             ^ ADDR
     ^ ADDR - MALLOC_ALIGN
*/

/* The amount of extra space used by the malloc header.  */
#define MALLOC_HEADER_SIZE	MALLOC_ALIGNMENT

/* Return base-address of a malloc allocation, given the user address.  */
#define MALLOC_BASE(addr)	((void *)((char *)addr - MALLOC_HEADER_SIZE))
/* Return the size of a malloc allocation, given the user address.  */
#define MALLOC_SIZE(addr)	(*(size_t *)MALLOC_BASE(addr))
/* Sets the size of a malloc allocation, given the user address.  */
#define MALLOC_SET_SIZE(addr, size)	(*(size_t *)MALLOC_BASE(addr) = (size))

/* Return the user address of a malloc allocation, given the base address.  */
#define MALLOC_ADDR(base)	((void *)((char *)base + MALLOC_HEADER_SIZE))


/* Locking for multithreaded apps.  */
#ifdef __UCLIBC_HAS_THREADS__

# include <pthread.h>

# define MALLOC_USE_LOCKING

typedef pthread_mutex_t malloc_mutex_t;
# define MALLOC_MUTEX_INIT	PTHREAD_MUTEX_INITIALIZER

/* The main malloc lock.  This must be hold while accessing __malloc_heap,
   and in order to gain __malloc_sbrk_lock.  */
extern malloc_mutex_t __malloc_lock;
# define __malloc_lock()	pthread_mutex_lock (&__malloc_lock)
# define __malloc_unlock()	pthread_mutex_unlock (&__malloc_lock)

# ifdef MALLOC_USE_SBRK
/* This lock is used to serialize uses of the `sbrk' function (in both
   malloc and free, sbrk may be used several times in succession, and
   things will break if these multiple calls are interleaved with another
   thread's use of sbrk!).  */
extern malloc_mutex_t __malloc_sbrk_lock;
#  define __malloc_lock_sbrk()	pthread_mutex_lock (&__malloc_sbrk_lock)
#  define __malloc_unlock_sbrk() pthread_mutex_unlock (&__malloc_sbrk_lock)
# endif /* MALLOC_USE_SBRK */

#else /* !__UCLIBC_HAS_THREADS__ */

/* Without threads, mutex operations are a nop.  */
# define __malloc_lock()	(void)0
# define __malloc_unlock()	(void)0
# define __malloc_lock_sbrk()	(void)0
# define __malloc_unlock_sbrk()	(void)0

#endif /* __UCLIBC_HAS_THREADS__ */


/* Use branch-prediction macros from libc if defined.  */
#ifdef likely
#define __malloc_likely(c)	likely(c)
#define __malloc_unlikely(c)	unlikely(c)
#else
#define __malloc_likely(c)	(c)
#define __malloc_unlikely(c)	(c)
#endif


/* Define MALLOC_DEBUGGING to cause malloc to emit debugging info to stderr.  */
#ifdef MALLOC_DEBUGGING
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
