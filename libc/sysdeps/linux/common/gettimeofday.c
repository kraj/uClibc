/* vi: set sw=4 ts=4: */
/*
 * gettimeofday() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codpoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <sys/time.h>

#ifdef __NR_gettimeofday
#define __NR___libc_gettimeofday __NR_gettimeofday
#endif
_syscall2(int, __libc_gettimeofday, struct timeval *, tv, struct timezone *, tz);
weak_alias(__libc_gettimeofday, gettimeofday);
