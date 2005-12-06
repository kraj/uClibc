/* vi: set sw=4 ts=4: */
/*
 * fchdir() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <unistd.h>
#define __NR___fchdir __NR_fchdir
attribute_hidden _syscall1(int, __fchdir, int, fd);
strong_alias(__fchdir,fchdir)
