/* vi: set sw=4 ts=4: */
/*
 * nice() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <unistd.h>
#ifdef __NR_nice
_syscall1(int, nice, int, inc);
#else
#include <sys/resource.h>
int nice(int incr)
{
	int save, prio, result;

	save = errno;
	__set_errno(0);
	prio = getpriority(PRIO_PROCESS, 0);
	if (prio == -1) {
		if (errno != 0) {
			return -1;
		} else {
			__set_errno(save);
		}
	}
	result = setpriority(PRIO_PROCESS, 0, prio + incr);
	if (result != -1) {
		return prio + incr;
	} else {
		return -1;
	}
}
#endif
