/* vi: set sw=4 ts=4: */
/*
 * stat() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

/* need to hide the 64bit prototype or the weak_alias()
 * will fail when __NR_stat64 doesnt exist */
#define stat64 __hidestat64
#define __stat64 __hide__stat64

#include "syscalls.h"
#include <unistd.h>
#include <sys/stat.h>
#include "xstatconv.h"

#undef stat64
#undef __stat64

#define __NR___syscall_stat __NR_stat
#undef __stat
#undef stat
static inline _syscall2(int, __syscall_stat,
		const char *, file_name, struct kernel_stat *, buf);

int attribute_hidden __stat(const char *file_name, struct stat *buf)
{
	int result;
	struct kernel_stat kbuf;

	result = __syscall_stat(file_name, &kbuf);
	if (result == 0) {
		__xstat_conv(&kbuf, buf);
	}
	return result;
}
strong_alias(__stat,stat)

#if ! defined __NR_stat64 && defined __UCLIBC_HAS_LFS__
hidden_strong_alias(__stat,__stat64)
weak_alias(__stat,stat64)
#endif
