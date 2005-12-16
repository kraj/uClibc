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

#define __NR___write __NR_write
attribute_hidden _syscall3(ssize_t, __write, int, fd, const __ptr_t, buf, size_t, count);
strong_alias(__write,write)
weak_alias(__write,__libc_write)
#if 0
/* Stupid libgcc.a from gcc 2.95.x uses __write in pure.o
 * which is a blatent GNU libc-ism... */
weak_alias(__libc_write, __write)
#endif
