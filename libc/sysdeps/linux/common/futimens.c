/* vi: set sw=4 ts=4: */
/*
 * futimens() implementation for uClibc
 *
 * Copyright (C) 2009 Bernhard Reutner-Fischer <uclibc@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#define __need_timespec
#include <time.h>
#ifdef __NR_utimensat
extern int utimensat (int __fd, __const char *__path,
	__const struct timespec __times[2],
	int __flags) __THROW;
libc_hidden_proto(utimensat)

int futimens (int fd, __const struct timespec ts[2])
{
	return utimensat(fd, 0, ts, 0);
}
#endif
