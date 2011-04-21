/* vi: set sw=4 ts=4: */
/*
 * open() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <fcntl.h>
#include <stdarg.h>
#include <cancel.h>

#define __NR___syscall_open __NR_open
static __always_inline _syscall3(int, __syscall_open, const char *, file,
				 int, flags, __kernel_mode_t, mode)
strong_alias_untyped(__syscall_open,__NC(open))

#define __NR___open2_nocancel __NR_open
_syscall2(int, __NC(open2), const char *, file, int, flags)

int open(const char *file, int oflag, ...)
{
	mode_t mode = 0;

	if (oflag & O_CREAT) {
		va_list arg;
		va_start(arg, oflag);
		mode = va_arg(arg, mode_t);
		va_end(arg);
	}

	if (SINGLE_THREAD_P)
		return __NC(open)(file, oflag, mode);
#ifdef __NEW_THREADS
	int oldtype = LIBC_CANCEL_ASYNC ();
	int result = __NC(open)(file, oflag, mode);
	LIBC_CANCEL_RESET (oldtype);
	return result;
#endif
}
lt_strong_alias(open)
lt_libc_hidden(open)
