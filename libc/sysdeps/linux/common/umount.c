/* vi: set sw=4 ts=4: */
/*
 * umount() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"

#ifdef __NR_umount	/* Some newer archs only have umount2 */
#include <sys/mount.h>
_syscall1(int, umount, const char *, specialfile);
#else
int umount(const char *special_file)
{
	__set_errno(ENOSYS);
	return -1;
}
#endif

