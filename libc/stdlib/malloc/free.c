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
#include <sys/mman.h>

#include "malloc.h"
#include "heap.h"


void free (void *mem)
{
  size_t size;

  mem = (size_t *)mem - 1;
  size = *(size_t *)mem;

  MALLOC_DEBUG ("free: 0x%lx (base = 0x%lx, total_size = %d)\n",
		(long)mem + sizeof (size_t), (long)mem, size);

  if (size >= MALLOC_MMAP_THRESHOLD)
    munmap (mem, size);
  else
    __heap_free (&__malloc_heap, mem, size);
}
