/*
 * libc/sysdeps/linux/v850/ftruncate64.c -- `ftruncate64' syscall for linux/v850
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

/* A version of ftruncate64 that passes in the 64-bit length argument as two
   32-bit arguments, as required by system call interface on the v850.  */
#define __NR__ftruncate64 __NR_ftruncate64
extern inline _syscall3 (int, _ftruncate64,
			 int, fd, unsigned long, len_lo, long, len_hi);

/* The exported ftruncate64.  */
int ftruncate64 (int fd, __off64_t length)
{
  return _ftruncate64 (fd, (unsigned long)length, (long)(length >> 32));
}

#endif /* __UCLIBC_HAVE_LFS__ */
