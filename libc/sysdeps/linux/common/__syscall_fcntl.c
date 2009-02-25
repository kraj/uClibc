/* vi: set sw=4 ts=4: */
/*
 * __syscall_fcntl() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <stdarg.h>
#include <fcntl.h>
#include <bits/wordsize.h>

#define __NR___syscall_fcntl __NR_fcntl
static __always_inline
_syscall3(int, __syscall_fcntl, int, fd, int, cmd, long, arg)

int fcntl(int fd, int cmd, ...)
{
	long arg;
	va_list list;

	va_start(list, cmd);
	arg = va_arg(list, long);
	va_end(list);

#if __WORDSIZE == 32
	if (cmd == F_GETLK64 || cmd == F_SETLK64 || cmd == F_SETLKW64) {
#if defined __UCLIBC_HAS_LFS__ && defined __NR_fcntl64
		return fcntl64(fd, cmd, arg);
#else
		__set_errno(ENOSYS);
		return -1;
#endif
	}
#endif

	return (__syscall_fcntl(fd, cmd, arg));
}
#ifndef __LINUXTHREADS_OLD__
libc_hidden_def(fcntl)
#else
libc_hidden_weak(fcntl)
strong_alias(fcntl,__libc_fcntl)
#endif

#if ! defined __NR_fcntl64 && defined __UCLIBC_HAS_LFS__
strong_alias(fcntl,fcntl64)
#endif
