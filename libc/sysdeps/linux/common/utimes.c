/* vi: set sw=4 ts=4: */
/*
 * utimes() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <sys/time.h>

#ifdef __NR_utimes
_syscall2(int, utimes, const char *, file, const struct timeval *, tvp)
#elif defined __NR_utime
# define __need_NULL
# include <stddef.h>
# include <utime.h>

int utimes(const char *file, const struct timeval tvp[2])
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
#if defined __NR_utimes || defined __NR_utime
libc_hidden_def(utimes)
#endif
