/* vi: set sw=4 ts=4: */
/*
 * get_kernel_syms() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"

struct kernel_sym;
#ifdef __NR_get_kernel_syms
_syscall1(int, get_kernel_syms, struct kernel_sym *, table);
#else
int get_kernel_syms(struct kernel_sym *table)
{
	__set_errno(ENOSYS);
	return -1;
}
#endif
