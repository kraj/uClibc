/* vi: set sw=4 ts=4: */
/*
 * mknod() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#define __NR___syscall_mknod __NR_mknod
/* kernel's fs/namei.c defines this:
 * long sys_mknod(const char __user *filename, int mode, unsigned dev),
 * so, no __kernel_mode_t and no __kernel_dev_t, please.
 */
static __inline__ _syscall3(int, __syscall_mknod,
		const char *, path,
		int /* __kernel_mode_t */, mode,
		unsigned /* __kernel_dev_t */, dev)

int mknod(const char *path, mode_t mode, dev_t dev)
{
	return __syscall_mknod(path, mode, dev);
}
libc_hidden_def(mknod)
