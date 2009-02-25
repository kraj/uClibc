/* vi: set sw=4 ts=4: */
/*
 * fsync() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <unistd.h>

#ifdef __LINUXTHREADS_OLD__
extern __typeof(fsync) weak_function fsync;
strong_alias(fsync,__libc_fsync)
#endif

_syscall1(int, fsync, int, fd)
