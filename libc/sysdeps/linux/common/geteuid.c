/* vi: set sw=4 ts=4: */
/*
 * geteuid() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>

libc_hidden_proto(geteuid)

#ifdef	__NR_geteuid
#define __NR___syscall_geteuid __NR_geteuid
static inline _syscall0(int, __syscall_geteuid);
uid_t geteuid(void)
{
	return (__syscall_geteuid());
}
#else
libc_hidden_proto(getuid)

uid_t geteuid(void)
{
	return (getuid());
}
#endif
libc_hidden_def(geteuid)
