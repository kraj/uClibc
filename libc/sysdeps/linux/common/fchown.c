/* vi: set sw=4 ts=4: */
/*
 * fchown() for uClibc
 *
 * Copyright (C) 2000-2006 by Erik Andersen <andersen@codepoet.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>
#include <linux/version.h>

/* Linux 2.3.39 introduced 32bit UID/GIDs.  Some platforms had 32
   bit type all along.  */
#if LINUX_VERSION_CODE >= 131879

_syscall3(int, fchown, int, fd, uid_t, owner, gid_t, group);

#else

#define __NR___syscall_fchown __NR_fchown
static inline _syscall3(int, __syscall_fchown, int, fd,
		__kernel_uid_t, owner, __kernel_gid_t, group);

int fchown(int fd, uid_t owner, gid_t group)
{
	if (((owner + 1) > (uid_t) ((__kernel_uid_t) - 1U))
		|| ((group + 1) > (gid_t) ((__kernel_gid_t) - 1U))) {
		__set_errno(EINVAL);
		return -1;
	}
	return (__syscall_fchown(fd, owner, group));
}

#endif
