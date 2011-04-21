/* vi: set sw=4 ts=4: */
/*
 * fork() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>

#if defined __NR_fork && defined __ARCH_USE_MMU__
# include <unistd.h>
# include <cancel.h>
_syscall0(pid_t, fork)
# ifdef __UCLIBC_HAS_THREADS__
strong_alias(fork,__libc_fork)
libc_hidden_weak(fork)
# else
libc_hidden_def(fork)
# endif
#endif
