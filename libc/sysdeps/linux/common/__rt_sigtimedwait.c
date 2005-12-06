/* vi: set sw=4 ts=4: */
/*
 * __rt_sigtimedwait() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <signal.h>
#define __need_NULL
#include <stddef.h>
#ifdef __NR_rt_sigtimedwait
#define __NR___rt_sigtimedwait __NR_rt_sigtimedwait
static _syscall4(int, __rt_sigtimedwait, const sigset_t *, set, siginfo_t *, info,
		  const struct timespec *, timeout, size_t, setsize);

int attribute_hidden __sigwaitinfo(const sigset_t * set, siginfo_t * info)
{
	return __rt_sigtimedwait(set, info, NULL, _NSIG / 8);
}

int attribute_hidden __sigtimedwait(const sigset_t * set, siginfo_t * info,
				 const struct timespec *timeout)
{
	return __rt_sigtimedwait(set, info, timeout, _NSIG / 8);
}
weak_alias (__sigtimedwait, sigtimedwait)
weak_alias (__sigwaitinfo, sigwaitinfo)
#else
int sigwaitinfo(const sigset_t * set, siginfo_t * info)
{
	if (set == NULL)
		__set_errno(EINVAL);
	else
		__set_errno(ENOSYS);
	return -1;
}

int sigtimedwait(const sigset_t * set, siginfo_t * info,
				 const struct timespec *timeout)
{
	if (set == NULL)
		__set_errno(EINVAL);
	else
		__set_errno(ENOSYS);
	return -1;
}
#endif
