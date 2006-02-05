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
