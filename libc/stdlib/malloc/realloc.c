/*
 * libc/stdlib/malloc/realloc.c -- realloc function
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
#include <string.h>

#include "malloc.h"
#include "heap.h"


void *
realloc (void *mem, size_t new_size)
{
  if (! mem)
    return malloc (new_size);
  else
    {
      void *base_mem = mem - MALLOC_ALIGNMENT;
      size_t size = *(size_t *)base_mem;

      MALLOC_DEBUG ("realloc: 0x%lx, %d (base = 0x%lx, total_size = %d)\n",
		    (long)mem, new_size, (long)base_mem, size);

      if (new_size <= size)
	return mem;
      else
	{
	  void *new_mem = 0;
	  size_t ext_size = new_size - size;
	  void *ext_addr = (char *)base_mem + ext_size;

	  ext_size = __heap_alloc_at (&__malloc_heap, ext_addr, ext_size);
	  if (! ext_size)
	    /* Our attempts to extend MEM in place failed, just
	       allocate-and-copy.  */
	    {
	      new_mem = malloc (new_size);
	      if (new_mem)
		{
		  memcpy (new_mem, mem, size);
		  free (mem);
		}
	    }

	  if (new_mem)
	    MALLOC_DEBUG ("  realloc: returning 0x%lx"
			  " (base:0x%lx, total_size:%d)\n",
			  (long)new_mem, (long)new_mem - sizeof(size_t), size);

	  return new_mem;
	}
    }
}
