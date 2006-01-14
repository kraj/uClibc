/* vi: set sw=4 ts=4: */
/*
 * getgid() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>

libc_hidden_proto(getgid)

#define __NR___syscall_getgid __NR_getgid
#if defined (__alpha__)
#define __NR_getgid     __NR_getxgid
#endif

static inline _syscall0(int, __syscall_getgid);
gid_t getgid(void)
{
	return (__syscall_getgid());
}
libc_hidden_def(getgid)
