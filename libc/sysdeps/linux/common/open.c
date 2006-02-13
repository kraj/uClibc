/* vi: set sw=4 ts=4: */
/*
 * open() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <sys/param.h>

extern __typeof(open) __libc_open;
libc_hidden_proto(__libc_open)

#define __NR___syscall_open __NR_open
static inline _syscall3(int, __syscall_open, const char *, file,
		int, flags, __kernel_mode_t, mode);

int __libc_open(const char *file, int flags, ...)
{
	/* gcc may warn about mode being uninitialized.
	 * Just ignore that, since gcc is wrong. */
	mode_t mode;

	if (flags & O_CREAT) {
		va_list ap;

		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}
	return __syscall_open(file, flags, mode);
}
libc_hidden_def(__libc_open)

libc_hidden_proto(open)
weak_alias(__libc_open,open)
libc_hidden_weak(open)

int creat(const char *file, mode_t mode)
{
	return __libc_open(file, O_WRONLY | O_CREAT | O_TRUNC, mode);
}
