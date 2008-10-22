/* vi: set sw=4 ts=4: */
/*
 * statfs() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <string.h>
#include <sys/param.h>
#include <sys/vfs.h>

extern __typeof(statfs) __libc_statfs;
libc_hidden_proto(__libc_statfs)
#define __NR___libc_statfs __NR_statfs
_syscall2(int, __libc_statfs, const char *, path, struct statfs *, buf)
libc_hidden_def(__libc_statfs)

#if defined __UCLIBC_LINUX_SPECIFIC__
libc_hidden_proto(statfs)
weak_alias(__libc_statfs,statfs)
libc_hidden_weak(statfs)
#endif
