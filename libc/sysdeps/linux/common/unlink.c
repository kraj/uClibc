/* vi: set sw=4 ts=4: */
/*
 * unlink() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <unistd.h>
#define __NR___unlink __NR_unlink
attribute_hidden _syscall1(int, __unlink, const char *, pathname);
strong_alias(__unlink,unlink)
