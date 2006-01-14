/* vi: set sw=4 ts=4: */
/*
 * execve() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>
#include <string.h>
#include <sys/param.h>

libc_hidden_proto(execve)

#define __NR___syscall_execve __NR_execve
static inline _syscall3(int, __syscall_execve, const char *, filename,
		  char *const *, argv, char *const *, envp);

int execve(const char * filename, char *const * argv, char *const * envp)
{
	return __syscall_execve(filename, argv, envp);
}
libc_hidden_def(execve)
