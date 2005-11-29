/* vi: set sw=4 ts=4: */
/*
 * getegid() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#define getgid __getgid

#include "syscalls.h"
#include <unistd.h>

#ifdef	__NR_getegid
#define __NR___syscall_getegid __NR_getegid
static inline _syscall0(int, __syscall_getegid);
gid_t attribute_hidden __getegid(void)
{
	return (__syscall_getegid());
}
#else
gid_t attribute_hidden __getegid(void)
{
	return (getgid());
}
#endif
strong_alias(__getegid,getegid)
