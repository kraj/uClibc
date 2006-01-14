/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/syscall.h>

libc_hidden_proto(setresuid)
libc_hidden_proto(setreuid)

int seteuid(uid_t uid)
{
    int result;

    if (uid == (uid_t) ~0)
    {
	__set_errno (EINVAL);
	return -1;
    }

#ifdef __NR_setresuid
    result = setresuid(-1, uid, -1);
    if (result == -1 && errno == ENOSYS)
	/* Will also set the saved user ID if euid != uid,
	 * making it impossible to switch back...*/
#endif
	result = setreuid(-1, uid);

    return result;
}
libc_hidden_proto(seteuid)
libc_hidden_def(seteuid)
