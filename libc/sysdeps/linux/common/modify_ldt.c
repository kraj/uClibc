/* vi: set sw=4 ts=4: */
/*
 * modify_ldt() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codpoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
_syscall3(int, modify_ldt, int, func, void *, ptr, unsigned long, bytecount);

weak_alias(modify_ldt, __modify_ldt);
