/*
 * libc/sysdeps/linux/v850/truncate64.c -- `truncate64' syscall for linux/v850
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

#include <features.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>

#ifdef __UCLIBC_HAVE_LFS__

/* A version of truncate64 that passes in the 64-bit length argument as two
   32-bit arguments, as required by system call interface on the v850.  */
#define __NR__truncate64 __NR_truncate64
extern inline _syscall3 (int, _truncate64,
			 const char *, file,
			 unsigned long, len_lo, long, len_hi);

/* The exported truncate64.  */
int truncate64 (const char *file, __off64_t length)
{
  return _truncate64 (file, (unsigned long)length, (long)(length >> 32));
}

#endif /* __UCLIBC_HAVE_LFS__ */
