/* vi: set sw=4 ts=4: */
/*
 * lseek() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>

extern __off_t __libc_lseek (int __fd, __off_t __offset, int __whence) __THROW;
libc_hidden_proto(__libc_lseek)

#define __NR___libc_lseek __NR_lseek
_syscall3(__off_t, __libc_lseek, int, fildes, __off_t, offset, int, whence);
libc_hidden_def(__libc_lseek)

strong_alias(__libc_lseek,lseek)
libc_hidden_proto(lseek)
libc_hidden_def(lseek)
