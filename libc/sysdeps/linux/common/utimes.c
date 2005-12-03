/* vi: set sw=4 ts=4: */
/*
 * utimes() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#define utime __utime

#include "syscalls.h"
#include <utime.h>
#ifdef __NR_utimes
#define __NR___utimes __NR_utimes
attribute_hidden _syscall2(int, __utimes, const char *, file, const struct timeval *, tvp);
#else
#include <stdlib.h>
#include <sys/time.h>
int attribute_hidden __utimes(const char *file, const struct timeval tvp[2])
{
	struct utimbuf buf, *times;

	if (tvp) {
		times = &buf;
		times->actime = tvp[0].tv_sec;
		times->modtime = tvp[1].tv_sec;
	} else {
		times = NULL;
	}
	return utime(file, times);
}
#endif
strong_alias(__utimes,utimes)
