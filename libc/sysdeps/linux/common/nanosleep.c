/* vi: set sw=4 ts=4: */
/*
 * nanosleep() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <time.h>

#if defined __USE_POSIX199309 && defined __NR_nanosleep
_syscall2(int, nanosleep, const struct timespec *, req,
		  struct timespec *, rem)
#ifndef __LINUXTHREADS_OLD__
libc_hidden_def(nanosleep)
#else
libc_hidden_weak(nanosleep)
strong_alias(nanosleep,__libc_nanosleep)
#endif
#endif
