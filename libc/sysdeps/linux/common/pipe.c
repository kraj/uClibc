/* vi: set sw=4 ts=4: */
/*
 * pipe() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <unistd.h>
#define __NR___pipe __NR_pipe
attribute_hidden _syscall1(int, __pipe, int *, filedes);
strong_alias(__pipe,pipe)
