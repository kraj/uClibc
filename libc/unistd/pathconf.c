/* pathconf -- adjusted for busybox
   Copyright (C) 1991,95,96,98,99,2000,2001 Free Software Foundation, Inc.
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

#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>


/* Get file-specific information about descriptor FD.  */
long int pathconf(const char *path, int name)
{
    int fd;

    if (path[0] == '\0')
    {
	__set_errno (ENOENT);
	return -1;
    }

    fd = open(path, O_RDONLY); 
    if (fd < 0) {
	__set_errno (ENOENT);
	return -1;
    }
    return (fpathconf(fd, name));
}

