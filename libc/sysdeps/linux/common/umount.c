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

#elif defined __NR_umount2
/* No umount syscall, but umount2 is available....  Try to
 * emulate umount() using umount2() */

#define __NR___syscall_umount2 __NR_umount2
static inline _syscall2(int, umount2, const char *, special_file, int, flags);

int umount(const char *special_file)
{
	return (__syscall_umount2(special_file, 0));
}

#else
int umount(const char *special_file)
{
	__set_errno(ENOSYS);
	return -1;
}
#endif
