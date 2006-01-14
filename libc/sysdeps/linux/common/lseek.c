/* vi: set sw=4 ts=4: */
/*
 * lseek() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <unistd.h>

#define __NR___libc_lseek __NR_lseek
_syscall3(__off_t, __libc_lseek, int, fildes, __off_t, offset, int, whence);
libc_hidden_proto(__libc_lseek)
libc_hidden_def(__libc_lseek)

strong_alias(__libc_lseek,lseek)
libc_hidden_proto(lseek)
libc_hidden_def(lseek)
