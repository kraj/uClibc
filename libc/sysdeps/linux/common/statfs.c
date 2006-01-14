/* vi: set sw=4 ts=4: */
/*
 * statfs() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <string.h>
#include <sys/param.h>
#include <sys/vfs.h>

libc_hidden_proto(statfs)

#define __NR___syscall_statfs __NR_statfs
static inline _syscall2(int, __syscall_statfs,
		const char *, path, struct statfs *, buf);

int statfs(const char *path, struct statfs * buf)
{
	return __syscall_statfs(path, buf);
}
libc_hidden_def(statfs)
