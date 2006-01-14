/* vi: set sw=4 ts=4: */
/*
 * fstat() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

/* need to hide the 64bit prototype or the weak_alias()
 * will fail when __NR_fstat64 doesnt exist */
#define fstat64 __hidefstat64
#define __fstat64 __hide__fstat64

#include "syscalls.h"
#include <unistd.h>
#include <sys/stat.h>
#include "xstatconv.h"

#undef fstat64
#undef __fstat64

#define __NR___syscall_fstat __NR_fstat
#undef __fstat
#undef fstat
static inline _syscall2(int, __syscall_fstat, int, fd, struct kernel_stat *, buf);

int attribute_hidden __fstat(int fd, struct stat *buf)
{
	int result;
	struct kernel_stat kbuf;

	result = __syscall_fstat(fd, &kbuf);
	if (result == 0) {
		__xstat_conv(&kbuf, buf);
	}
	return result;
}
strong_alias(__fstat,fstat)

#if ! defined __NR_fstat64 && defined __UCLIBC_HAS_LFS__
hidden_strong_alias(__fstat,__fstat64)
weak_alias(__fstat,fstat64)
#endif
