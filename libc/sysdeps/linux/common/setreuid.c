/* vi: set sw=4 ts=4: */
/*
 * setreuid() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>

libc_hidden_proto(setreuid)

#define __NR___syscall_setreuid __NR_setreuid
static inline _syscall2(int, __syscall_setreuid,
		__kernel_uid_t, ruid, __kernel_uid_t, euid);

int setreuid(uid_t ruid, uid_t euid)
{
	if (((ruid + 1) > (uid_t) ((__kernel_uid_t) - 1U))
		|| ((euid + 1) > (uid_t) ((__kernel_uid_t) - 1U))) {
		__set_errno(EINVAL);
		return -1;
	}
	return (__syscall_setreuid(ruid, euid));
}
libc_hidden_def(setreuid)
