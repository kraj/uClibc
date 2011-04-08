/* vi: set sw=4 ts=4: */
/*
 * getpid() for uClibc
 *
 * Copyright (C) 2000-2006 by Erik Andersen <andersen@codepoet.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <unistd.h>

#ifdef __NR_getxpid
# undef __NR_getpid
# define __NR_getpid __NR_getxpid
#endif

#ifdef IS_IN_rtld
# define __NR__dl_getpid __NR_getpid
# define getpid _dl_getpid
static __always_inline
#endif
_syscall_noerr0(pid_t, getpid)
libc_hidden_weak(getpid)
#if !defined __NR_getppid && !defined IS_IN_rtld
strong_alias(getpid,getppid)
#endif
