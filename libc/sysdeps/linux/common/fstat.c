/* vi: set sw=4 ts=4: */
/*
 * fstat() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <unistd.h>
#include <sys/stat.h>
#include "xstatconv.h"

#define __NR___syscall_fstat __NR_fstat
static __inline__ _syscall2(int, __syscall_fstat, int, fd, struct kernel_stat *, buf)

int fstat(int fd, struct stat *buf)
{
	int result;
	struct kernel_stat kbuf;

	result = __syscall_fstat(fd, &kbuf);
	if (result == 0) {
		__xstat_conv(&kbuf, buf);
	}
	return result;
}
libc_hidden_def(fstat)

#if ! defined __NR_fstat64 && defined __UCLIBC_HAS_LFS__
strong_alias_untyped(fstat,fstat64)
libc_hidden_def(fstat64)
#endif
