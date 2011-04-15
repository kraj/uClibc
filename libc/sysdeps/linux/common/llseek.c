/* vi: set sw=4 ts=4: */
/*
 * llseek/lseek64 syscall for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <_lfs_64.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>

/* Newer kernel ports have llseek() instead of _llseek() */
#if !defined __NR__llseek && defined __NR_llseek
# define __NR__llseek __NR_llseek
#endif

#ifdef __NR__llseek
off64_t lseek64(int fd, off64_t offset, int whence)
{
	off64_t result;
	return (off64_t)INLINE_SYSCALL(_llseek, 5, fd, (off_t) (offset >> 32),
				(off_t) (offset & 0xffffffff), &result, whence) ?: result;
}
#else
off64_t lseek64(int fd, off64_t offset, int whence)
{
	return (off64_t)lseek(fd, (off_t) (offset), whence);
}
#endif
#ifndef __LINUXTHREADS_OLD__
libc_hidden_def(lseek64)
#else
libc_hidden_weak(lseek64)
strong_alias(lseek64,__libc_lseek64)
#endif
