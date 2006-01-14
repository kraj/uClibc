/* vi: set sw=4 ts=4: */
/*
 * getuid() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>

libc_hidden_proto(getuid)

#if defined (__alpha__)
#define __NR_getuid     __NR_getxuid
#endif
#define __NR___syscall_getuid __NR_getuid

static inline _syscall0(int, __syscall_getuid);

uid_t getuid(void)
{
	return (__syscall_getuid());
}
libc_hidden_def(getuid)
