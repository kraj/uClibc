/* vi: set sw=4 ts=4: */
/*
 * llseek/lseek64 syscall for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#if defined __NR__llseek && defined __UCLIBC_HAS_LFS__

# ifndef INLINE_SYSCALL
#  define INLINE_SYSCALL(name, nr, args...) __syscall_llseek (args)
#  define __NR___syscall_llseek __NR__llseek
static __inline__ _syscall5(int, __syscall_llseek, int, fd, off_t, offset_hi,
		off_t, offset_lo, loff_t *, result, int, whence)
# endif

loff_t lseek64(int fd, loff_t offset, int whence)
{
	loff_t result;
	return (loff_t)(INLINE_SYSCALL(_llseek, 5, fd, (off_t) (offset >> 32),
				(off_t) (offset & 0xffffffff), &result, whence) ?: result);
}

#else

loff_t lseek64(int fd, loff_t offset, int whence)
{
	return (loff_t)(lseek(fd, (off_t) (offset), whence));
}

#endif

#ifndef __LINUXTHREADS_OLD__
libc_hidden_def(lseek64)
#else
libc_hidden_weak(lseek64)
strong_alias(lseek64,__libc_lseek64)
#endif
