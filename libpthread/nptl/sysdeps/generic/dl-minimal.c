/* Minimal replacements for basic facilities used in the dynamic linker.
   Copyright (C) 1995-1998,2000-2002,2004 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

//#include <errno.h>
#include <limits.h>
#include <string.h>
#include <tls.h>
//#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <ldsodefs.h>

/* Minimal `malloc' allocator for use while loading shared libraries.
   No block is ever freed.  */

static void *alloc_ptr, *alloc_end, *alloc_last_block;

/* Declarations of global functions.  */
extern void weak_function free (void *ptr);
extern void * weak_function realloc (void *ptr, size_t n);

/* Allocate an aligned memory block.  */
void * weak_function
memalign (size_t align, size_t n)
{
#ifdef MAP_ANON
#define	_dl_zerofd (-1)
#else
  extern int _dl_zerofd;

  if (_dl_zerofd == -1)
    _dl_zerofd = _dl_sysdep_open_zero_fill ();
#define MAP_ANON 0
#endif

  if (alloc_end == 0)
    {
      /* Consume any unused space in the last page of our data segment.  */
      extern int _end attribute_hidden;
      alloc_ptr = &_end;
      alloc_end = (void *) 0 + (((alloc_ptr - (void *) 0)
				 + GLRO(dl_pagesize) - 1)
				& ~(GLRO(dl_pagesize) - 1));
    }

  /* Make sure the allocation pointer is ideally aligned.  */
  alloc_ptr = (void *) 0 + (((alloc_ptr - (void *) 0) + align - 1)
			    & ~(align - 1));

  if (alloc_ptr + n >= alloc_end)
    {
      /* Insufficient space left; allocate another page.  */
      caddr_t page;
      size_t nup = (n + GLRO(dl_pagesize) - 1) & ~(GLRO(dl_pagesize) - 1);
      page = _dl_mmap (0, nup, PROT_READ|PROT_WRITE,
		     MAP_ANON|MAP_PRIVATE, _dl_zerofd, 0);
      if (_dl_mmap_check_error(page))
	return -1;
      if (page != alloc_end)
	alloc_ptr = page;
      alloc_end = page + nup;
    }

  alloc_last_block = (void *) alloc_ptr;
  alloc_ptr += n;
  return alloc_last_block;
}

void * weak_function
malloc (size_t n)
{
  return memalign (sizeof (double), n);
}

/* We use this function occasionally since the real implementation may
   be optimized when it can assume the memory it returns already is
   set to NUL.  */
void * weak_function
calloc (size_t nmemb, size_t size)
{
  /* New memory from the trivial malloc above is always already cleared.
     (We make sure that's true in the rare occasion it might not be,
     by clearing memory in free, below.)  */
  return malloc (nmemb * size);
}

/* This will rarely be called.  */
void weak_function
free (void *ptr)
{
  /* We can free only the last block allocated.  */
  if (ptr == alloc_last_block)
    {
      /* Since this is rare, we clear the freed block here
	 so that calloc can presume malloc returns cleared memory.  */
      _dl_memset (alloc_last_block, '\0', alloc_ptr - alloc_last_block);
      alloc_ptr = alloc_last_block;
    }
}

/* This is only called with the most recent block returned by malloc.  */
void * weak_function
realloc (void *ptr, size_t n)
{
  void *new;
  if (ptr == NULL)
    return malloc (n);
  assert (ptr == alloc_last_block);
  alloc_ptr = alloc_last_block;
  new = malloc (n);
  assert (new == ptr);
  return new;
}
