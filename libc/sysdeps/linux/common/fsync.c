/* vi: set sw=4 ts=4: */
/*
 * fsync() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>
#ifdef __UCLIBC__HAS_THREADS__
# include <sysdep-cancel.h>
#endif

extern __typeof(fsync) __libc_fsync;
#ifdef __UCLIBC__HAS_THREADS__
int __libc_fsync (int fd)
{
	if (SINGLE_THREAD_P)
		return INLINE_SYSCALL (fsync, 1, fd);

	int oldtype = LIBC_CANCEL_ASYNC ();

	int result = INLINE_SYSCALL (fsync, 1, fd);

	LIBC_CANCEL_RESET (oldtype);

	return result;
}
#else
# define __NR___libc_fsync __NR_fsync
_syscall1(int, __libc_fsync, int, fd);
#endif
weak_alias(__libc_fsync, fsync)
