/* vi: set sw=4 ts=4: */
/*
 * rmdir() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <unistd.h>
#define __NR___rmdir __NR_rmdir
attribute_hidden _syscall1(int, __rmdir, const char *, pathname);
strong_alias(__rmdir,rmdir)
