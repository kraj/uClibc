/* vi: set sw=4 ts=4: */
/*
 * utime() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#define utimes __utimes

#include "syscalls.h"
#include <utime.h>
#ifdef __NR_utime
#define __NR___utime __NR_utime
attribute_hidden _syscall2(int, __utime, const char *, file, const struct utimbuf *, times);
#else
#include <stdlib.h>
#include <sys/time.h>
int attribute_hidden __utime(const char *file, const struct utimbuf *times)
{
	struct timeval timevals[2];

	if (times != NULL) {
		timevals[0].tv_usec = 0L;
		timevals[1].tv_usec = 0L;
		timevals[0].tv_sec = (long int) times->actime;
		timevals[1].tv_sec = (long int) times->modtime;
	} else {
		if (gettimeofday(&timevals[0], NULL) < 0) {
			return -1;
		}
		timevals[1] = timevals[0];
	}
	return utimes(file, timevals);
}
#endif
strong_alias(__utime,utime)
