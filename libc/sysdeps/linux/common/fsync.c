/* vi: set sw=4 ts=4: */
/*
 * fsync() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>

#define __NR___libc_fsync __NR_fsync
_syscall1(int, __libc_fsync, int, fd);
strong_alias(__libc_fsync, fsync)
