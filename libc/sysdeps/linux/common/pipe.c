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
_syscall1(int, pipe, int *, filedes);
libc_hidden_proto(pipe)
libc_hidden_def(pipe)
