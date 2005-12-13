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

#define __NR___lseek __NR_lseek
attribute_hidden _syscall3(__off_t, __lseek, int, fildes, __off_t, offset, int, whence);
strong_alias(__lseek,lseek)
weak_alias(__lseek,__libc_lseek)
