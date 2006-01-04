/* vi: set sw=4 ts=4: */
/*
 * query_module() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#ifdef __NR_query_module
_syscall5(int, query_module, const char *, name, int, which,
		  void *, buf, size_t, bufsize, size_t *, ret);
#else
int query_module(const char *name attribute_unused, int which attribute_unused,
				 void *buf attribute_unused, size_t bufsize attribute_unused, size_t * ret attribute_unused)
{
	__set_errno(ENOSYS);
	return -1;
}
#endif
