/* Copyright (C) 1993, 95, 96, 97, 98, 99 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>

#include <sysdep.h>

#include <linux/posix_types.h>

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)


extern int _getdents __P ((int fd, char *buf, size_t nbytes));

/* For Linux we need a special version of this file since the
   definition of `struct dirent' is not the same for the kernel and
   the libc.  There is one additional field which might be introduced
   in the kernel structure in the future.
   */
#define dirent kernel_dirent
#include <linux/dirent.h>
#undef dirent


#ifdef GETDENTS64
# define __getdents __getdents64
# define dirent dirent64
#endif

/* The problem here is that we cannot simply read the next NBYTES
   bytes.  We need to take the additional field into account.  We use
   some heuristic.  Assuming the directory contains names with 14
   characters on average we can compute an estimated number of entries
   which fit in the buffer.  Taking this number allows us to specify a
   reasonable number of bytes to read.  If we should be wrong, we can
   reset the file descriptor.  In practice the kernel is limiting the
   amount of data returned much more then the reduced buffer size.  */
ssize_t getdents (int fd, char *buf, size_t nbytes)
{
  off_t last_offset = 0;
  size_t red_nbytes;
  struct kernel_dirent *skdp, *kdp;
  struct dirent *dp;
  int retval;
  const size_t size_diff = (offsetof (struct dirent, d_name)
			    - offsetof (struct kernel_dirent, d_name));

  red_nbytes = nbytes - ((nbytes / (offsetof (struct dirent, d_name) + 14))
			 * size_diff);

  dp = (struct dirent *) buf;
  skdp = kdp = malloc (red_nbytes);

  retval = _getdents(fd, (char *) kdp, red_nbytes);

  if (retval == -1)
    return -1;

  while ((char *) kdp < (char *) skdp + retval)
    {
      const size_t alignment = __alignof__ (struct dirent);
      /* Since kdp->d_reclen is already aligned for the kernel structure
	 this may compute a value that is bigger than necessary.  */
      size_t new_reclen = ((kdp->d_reclen + size_diff + alignment - 1)
			   & ~(alignment - 1));
      if ((char *) dp + new_reclen > buf + nbytes)
	{
	  /* Our heuristic failed.  We read too many entries.  Reset
	     the stream.  `last_offset' contains the last known
	     position.  If it is zero this is the first record we are
	     reading.  In this case do a relative search.  */
	  if (last_offset == 0)
	    lseek (fd, -retval, SEEK_CUR);
	  else
	    lseek (fd, last_offset, SEEK_SET);
	  break;
	}

      last_offset = kdp->d_off;
      dp->d_ino = kdp->d_ino;
      dp->d_off = kdp->d_off;
      dp->d_reclen = new_reclen;
      dp->d_type = DT_UNKNOWN;
      memcpy (dp->d_name, kdp->d_name,
	      kdp->d_reclen - offsetof (struct kernel_dirent, d_name));

      dp = (struct dirent *) ((char *) dp + new_reclen);
      kdp = (struct kernel_dirent *) (((char *) kdp) + kdp->d_reclen);
    }

  return (char *) dp - buf;
}
