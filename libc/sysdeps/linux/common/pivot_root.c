/* vi: set sw=4 ts=4: */
/*
 * pivot_root() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
_syscall2(int, pivot_root, const char *, new_root, const char *, put_old);
