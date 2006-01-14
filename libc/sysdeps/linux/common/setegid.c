/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/syscall.h>

libc_hidden_proto(setresgid)
libc_hidden_proto(setregid)

int setegid(gid_t gid)
{
    int result;

    if (gid == (gid_t) ~0)
    {
	__set_errno (EINVAL);
	return -1;
    }

#ifdef __NR_setresgid
    result = setresgid(-1, gid, -1);
    if (result == -1 && errno == ENOSYS)
	/* Will also set the saved group ID if egid != gid,
	 * making it impossible to switch back...*/
#endif
	result = setregid(-1, gid);

    return result;
}
