/* Provide kernel hint to read ahead.
   Copyright (C) 2002, 2003, 2004 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <sys/syscall.h>

#if defined __NR_readahead && defined __UCLIBC_HAS_LFS__ && defined __USE_GNU

# include <fcntl.h>
# include <bits/wordsize.h>

# define __NR___readahead __NR_readahead

# if __WORDSIZE == 64

static __inline__ _syscall3(ssize_t, __readahead, int, fd,
			    off_t, offset, size_t, count)

ssize_t readahead(int fd, off_t offset, size_t count)
{
	return __readahead(fd, offset, count);
}

# else

static __inline__ _syscall4(ssize_t, __readahead, int, fd,
			    off_t, high_offset, off_t, low_offset, size_t, count)

ssize_t readahead(int fd, off64_t offset, size_t count)
{
	return __readahead(fd, (off_t) (offset >> 32), (off_t) (offset & 0xffffffff), count);
}

# endif

#endif
