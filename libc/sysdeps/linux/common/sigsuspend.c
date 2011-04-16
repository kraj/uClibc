/* vi: set sw=4 ts=4: */
/*
 * sigsuspend() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>

#ifdef __USE_POSIX
#include <signal.h>

#ifdef __UCLIBC_HAS_THREADS_NATIVE__
# ifndef __NR_rt_sigsuspend
#  error break build, NPTL needs rt_sigsuspend syscall
# endif
# include <sysdep-cancel.h>
#else
# define SINGLE_THREAD_P 1
#endif

#ifdef __NR_rt_sigsuspend
static _syscall2(int, rt_sigsuspend, const sigset_t *, set, size_t, size)

/* Change the set of blocked signals to SET,
   wait until a signal arrives, and restore the set of blocked signals.  */
int sigsuspend(const sigset_t *set)
{
	if (SINGLE_THREAD_P)
		return rt_sigsuspend(set, _NSIG / 8);

# ifdef __UCLIBC_HAS_THREADS_NATIVE__
	int oldtype = LIBC_CANCEL_ASYNC ();
	int result = rt_sigsuspend(set, _NSIG / 8);
	LIBC_CANCEL_RESET (oldtype);
	return result;
# endif
}
#else
# define __NR___syscall_sigsuspend __NR_sigsuspend
static __always_inline _syscall3(int, __syscall_sigsuspend, int, a, unsigned long int, b,
				 unsigned long int, c)

int sigsuspend(const sigset_t *set)
{
	return __syscall_sigsuspend(0, 0, set->__val[0]);
}
#endif
libc_hidden_def(sigsuspend)
#endif
