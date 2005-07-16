/* Convert between the kernel's `struct stat' format, and libc's.
   Copyright (C) 1991,1995,1996,1997,2000,2002 Free Software Foundation, Inc.
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
   02111-1307 USA. 
   
   Modified for uClibc by Erik Andersen <andersen@codepoet.org>
   */

#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE
#include <features.h>
#undef __OPTIMIZE__
/* We absolutely do _NOT_ want interfaces silently
 *  *  * renamed under us or very bad things will happen... */
#ifdef __USE_FILE_OFFSET64
# undef __USE_FILE_OFFSET64
#endif
#include <sys/stat.h>
#include "xstatconv.h"

void __xstat_conv(struct kernel_stat *kbuf, struct stat *buf)
{
    /* Convert to current kernel version of `struct stat'.  */
    buf->st_dev = kbuf->st_dev;
    buf->st_ino = kbuf->st_ino;
    buf->st_mode = kbuf->st_mode;
    buf->st_nlink = kbuf->st_nlink;
    buf->st_uid = kbuf->st_uid;
    buf->st_gid = kbuf->st_gid;
    buf->st_rdev = kbuf->st_rdev;
    buf->st_size = kbuf->st_size;
    buf->st_blksize = kbuf->st_blksize;
    buf->st_blocks = kbuf->st_blocks;
    buf->st_atime = kbuf->st_atime;
    buf->st_mtime = kbuf->st_mtime;
    buf->st_ctime = kbuf->st_ctime;
#ifdef STAT_HAVE_NSEC
	buf->st_atime_nsec = kbuf->st_atime_nsec;
	buf->st_mtime_nsec = kbuf->st_mtime_nsec;
	buf->st_ctime_nsec = kbuf->st_ctime_nsec;
#endif
}

#if defined(__UCLIBC_HAS_LFS__)

/* OK, this is ugly, but not much we can do about it.
 *
 * The issue at hand is that the kernel introduced an ABI change between 
 * 2.4 and 2.6 for big endian machines on some architectures in stat64 with 
 * st_dev and st_rdev fields.   Older kernels used 'short' but newer kernels 
 * use a 'long'.  As if that wasn't fun enough, the location of st_blocks and 
 * its padding have swapped.  So, when applicable, we need to detect and shift 
 * bits around and hope for the best.  The kernel at least helps us out a bit 
 * because it will be sure to zero out all padding fields.
 *
 * Unimportant Note About Mis-detections:
 * - if user is running a 2.4 system with the old ABI and they stat a file on 
 *   a system which major 0 / minor 0.  We can ignore this though because the 
 *   kernel has major 0 reserved for non mountable devices (wh00t!)
 * - if user is running a 2.6 system with the new ABI and they stat a file on 
 *   a device whose st_dev fills up the lower 6 bytes into the upper 2 bytes 
 *   which overlap.  Again we should be able to safely ignore this because 
 *   that means it'd be a pretty big ass (read: uncommon) major/minor combo :).

 Old [2.4]:
struct stat64 {
    unsigned short  st_dev;
    unsigned char   __pad0[10];
    ...
    unsigned long   st_blocks;
    unsigned long   __pad4;
    ...
 New [2.6]:
struct stat64 {
    unsigned long long  st_dev;
    unsigned char   __pad0[4];
    ...
    unsigned long   __pad4;
    unsigned long   st_blocks;
    ...

 * sizeof(unsigned short) = 2
 * sizeof(unsigned long long) = 8
 */
#include <endian.h>
#include <sys/utsname.h>
#if BYTE_ORDER == BIG_ENDIAN && \
    (defined(__ARMEB__)) /* || defined(__sh__) || defined(__sparc__))*/
# define NEED_ABI_CHECK 1
#else
# define NEED_ABI_CHECK 0
#endif

void __xstat64_conv(struct kernel_stat64 *kbuf, struct stat64 *buf)
{
    /* Convert to current kernel version of `struct stat64'.  */

#if NEED_ABI_CHECK
	if (!kbuf->st_dev.old_abi) {
		/* new 2.6 ABI */
		buf->st_dev = kbuf->st_dev.new_abi;
		buf->st_rdev = kbuf->st_rdev.new_abi;
		buf->st_blocks = kbuf->st_blocks;
	} else {
		/* old 2.4 ABI */
		buf->st_dev = kbuf->st_dev.old_abi;
		buf->st_rdev = kbuf->st_dev.old_abi;
		buf->st_blocks = kbuf->__pad_st_blocks;
	}
#else
    buf->st_dev = kbuf->st_dev;
    buf->st_ino = kbuf->st_ino;
    buf->st_rdev = kbuf->st_rdev;
#endif

#ifdef _HAVE_STAT64___ST_INO
    buf->__st_ino = kbuf->__st_ino;
#endif

    buf->st_mode = kbuf->st_mode;
    buf->st_nlink = kbuf->st_nlink;
    buf->st_uid = kbuf->st_uid;
    buf->st_gid = kbuf->st_gid;
    buf->st_size = kbuf->st_size;
    buf->st_blksize = kbuf->st_blksize;
    buf->st_atime = kbuf->st_atime;
    buf->st_mtime = kbuf->st_mtime;
    buf->st_ctime = kbuf->st_ctime;

#ifdef STAT_HAVE_NSEC
	buf->st_atime_nsec = kbuf->st_atime_nsec;
	buf->st_mtime_nsec = kbuf->st_mtime_nsec;
	buf->st_ctime_nsec = kbuf->st_ctime_nsec;
#endif
}
#endif
