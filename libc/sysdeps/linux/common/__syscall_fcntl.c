/* vi: set sw=4 ts=4: */
/*
 * __syscall_fcntl() for uClibc
 *
 * Copyright (C) 2006 Steven J. Hill <sjhill@realitydiluted.com>
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <stdarg.h>
#include <cancel.h>	/* Must come before <fcntl.h>.  */
#include <fcntl.h>
#include <bits/wordsize.h>

int __NC(fcntl)(int fd, int cmd, long arg)
{
#if __WORDSIZE == 32
	if (cmd == F_GETLK64 || cmd == F_SETLK64 || cmd == F_SETLKW64) {
# if defined __UCLIBC_HAS_LFS__ && defined __NR_fcntl64
		return __NC(fcntl64)(fd, cmd, arg);
# else
		__set_errno(ENOSYS);
		return -1;
# endif
	}
#endif
	return INLINE_SYSCALL(fcntl, 3, fd, cmd, arg);
}

int fcntl(int fd, int cmd, ...)
{
	va_list ap;
	long arg;

	va_start (ap, cmd);
	arg = va_arg (ap, long);
	va_end (ap);

	if (SINGLE_THREAD_P || (cmd != F_SETLKW && cmd != F_SETLKW64))
		return __NC(fcntl)(fd, cmd, arg);
#ifdef __NEW_THREADS
	int oldtype = LIBC_CANCEL_ASYNC ();
	int result = __NC(fcntl)(fd, cmd, arg);
	LIBC_CANCEL_RESET (oldtype);
	return result;
#endif
}
lt_strong_alias(fcntl)
lt_libc_hidden(fcntl)
#if defined __UCLIBC_HAS_LFS__ && !defined __NR_fcntl64 && __WORDSIZE == 32
strong_alias_untyped(fcntl,fcntl64)
lt_strong_alias(fcntl64)
lt_libc_hidden(fcntl64)
#endif
