/* vi: set sw=4 ts=4: */
/*
 * fstatfs() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <sys/vfs.h>
#define __NR___fstatfs __NR_fstatfs
attribute_hidden _syscall2(int, __fstatfs, int, fd, struct statfs *, buf);
strong_alias(__fstatfs,fstatfs)
