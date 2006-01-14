/* vi: set sw=4 ts=4: */
/*
 * write() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <unistd.h>

#define __NR___libc_write __NR_write
_syscall3(ssize_t, __libc_write, int, fd, const __ptr_t, buf, size_t, count);
strong_alias(__libc_write,write)
libc_hidden_proto(write)
libc_hidden_def(write)
#if 0
/* Stupid libgcc.a from gcc 2.95.x uses __write in pure.o
 * which is a blatent GNU libc-ism... */
strong_alias(__libc_write,__write)
#endif
