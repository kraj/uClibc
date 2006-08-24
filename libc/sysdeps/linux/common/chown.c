/* vi: set sw=4 ts=4: */
/*
 * chown() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>

libc_hidden_proto(chown)

#define __NR___syscall_chown __NR_chown
static inline _syscall3(int, __syscall_chown, const char *, path,
		__kernel_uid_t, owner, __kernel_gid_t, group);

int chown(const char *path, uid_t owner, gid_t group)
{
	if (((owner + 1) > (uid_t) ((__kernel_uid_t) - 1U))
		|| ((group + 1) > (gid_t) ((__kernel_gid_t) - 1U))) {
		__set_errno(EINVAL);
		return -1;
	}
	return (__syscall_chown(path, owner, group));
}
libc_hidden_def(chown)
