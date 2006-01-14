/* vi: set sw=4 ts=4: */
/*
 * setfsgid() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <sys/fsuid.h>

#define __NR___syscall_setfsgid __NR_setfsgid
static inline _syscall1(int, __syscall_setfsgid, __kernel_gid_t, gid);

int setfsgid(gid_t gid)
{
	if (gid != (gid_t) ((__kernel_gid_t) gid)) {
		__set_errno(EINVAL);
		return -1;
	}
	return (__syscall_setfsgid(gid));
}
