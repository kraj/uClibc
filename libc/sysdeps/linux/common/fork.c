/* vi: set sw=4 ts=4: */
/*
 * fork() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <unistd.h>

#ifdef __ARCH_HAS_MMU__
#ifdef __NR_fork
#define __NR___fork __NR_fork
attribute_hidden _syscall0(pid_t, __fork);
strong_alias(__fork,fork)
weak_alias(__fork,__libc_fork)
#endif
#endif
