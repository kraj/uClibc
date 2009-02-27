/* vi: set sw=4 ts=4: */
/*
 * lstat() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <unistd.h>
#include <sys/stat.h>
#include "xstatconv.h"

#define __NR___syscall_lstat __NR_lstat
static __inline__ _syscall2(int, __syscall_lstat,
		const char *, file_name, struct kernel_stat *, buf)

int lstat(const char *file_name, struct stat *buf)
{
	int result;
	struct kernel_stat kbuf;

	result = __syscall_lstat(file_name, &kbuf);
	if (result == 0) {
		__xstat_conv(&kbuf, buf);
	}
	return result;
}
libc_hidden_def(lstat)

#if ! defined __NR_lstat64 && defined __UCLIBC_HAS_LFS__
strong_alias_untyped(lstat,lstat64)
libc_hidden_def(lstat64)
#endif
