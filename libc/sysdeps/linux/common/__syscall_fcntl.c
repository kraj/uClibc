/* vi: set sw=4 ts=4: */
/*
 * __syscall_fcntl() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <stdarg.h>
#include <fcntl.h>

#define __NR___syscall_fcntl __NR_fcntl
#ifdef __UCLIBC_HAS_LFS__
static inline
#endif
_syscall3(int, __syscall_fcntl, int, fd, int, cmd, long, arg);

int __libc_fcntl(int fd, int cmd, ...)
{
	long arg;
	va_list list;

	if (cmd == F_GETLK64 || cmd == F_SETLK64 || cmd == F_SETLKW64) {
		__set_errno(ENOSYS);
		return -1;
	}
	va_start(list, cmd);
	arg = va_arg(list, long);
	va_end(list);
	return (__syscall_fcntl(fd, cmd, arg));
}

weak_alias(__libc_fcntl, fcntl);
#if ! defined __NR_fcntl64 && defined __UCLIBC_HAS_LFS__
weak_alias(__libc_fcntl, fcntl64);
#endif
