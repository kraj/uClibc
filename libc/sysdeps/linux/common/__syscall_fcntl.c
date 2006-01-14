/* vi: set sw=4 ts=4: */
/*
 * __syscall_fcntl() for uClibc
 *
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <stdarg.h>
#include <fcntl.h>

#undef __fcntl

#if defined __UCLIBC_HAS_LFS__ && defined __NR_fcntl64
extern int __fcntl64(int fd, int cmd, ...) attribute_hidden;
#endif

#undef fcntl
#define __NR___syscall_fcntl __NR_fcntl
static inline
_syscall3(int, __syscall_fcntl, int, fd, int, cmd, long, arg);

int attribute_hidden __fcntl(int fd, int cmd, ...)
{
	long arg;
	va_list list;

	va_start(list, cmd);
	arg = va_arg(list, long);
	va_end(list);

	if (cmd == F_GETLK64 || cmd == F_SETLK64 || cmd == F_SETLKW64) {
#if defined __UCLIBC_HAS_LFS__ && defined __NR_fcntl64
		return __fcntl64(fd, cmd, arg);
#else
		__set_errno(ENOSYS);
		return -1;
#endif
	}
	return (__syscall_fcntl(fd, cmd, arg));
}
strong_alias(__fcntl,fcntl)
weak_alias(__fcntl,__libc_fcntl)
#if ! defined __NR_fcntl64 && defined __UCLIBC_HAS_LFS__
hidden_strong_alias(__fcntl,__fcntl64)
weak_alias(__fcntl,fcntl64)
weak_alias(__fcntl,__libc_fcntl64)
#endif
