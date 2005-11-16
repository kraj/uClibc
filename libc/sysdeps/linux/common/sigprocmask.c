/* vi: set sw=4 ts=4: */
/*
 * sigprocmask() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <signal.h>

#undef sigprocmask

#ifdef __NR_rt_sigprocmask

#define __NR___rt_sigprocmask __NR_rt_sigprocmask
_syscall4(int, __rt_sigprocmask, int, how, const sigset_t *, set,
		  sigset_t *, oldset, size_t, size);

int __sigprocmask(int how, const sigset_t * set, sigset_t * oldset)
{
#ifdef SIGCANCEL
	sigset_t local_newmask;

	/*
	 * The only thing we have to make sure here is that SIGCANCEL and
	 * SIGSETXID are not blocked.
	 */
	if (set != NULL && (__builtin_expect (__sigismember (set, SIGCANCEL), 0)
# ifdef SIGSETXID
		|| __builtin_expect (__sigismember (set, SIGSETXID), 0)
# endif
		))
	{
		local_newmask = *set;
		__sigdelset (&local_newmask, SIGCANCEL);
# ifdef SIGSETXID
		__sigdelset (&local_newmask, SIGSETXID);
# endif
		set = &local_newmask;
	}
#endif

	if (set &&
#if (SIG_BLOCK == 0) && (SIG_UNBLOCK == 1) && (SIG_SETMASK == 2)
		(((unsigned int) how) > 2)
#else
#warning "compile time assumption violated.. slow path..."
		((how != SIG_BLOCK) && (how != SIG_UNBLOCK)
		 && (how != SIG_SETMASK))
#endif
		) {
		__set_errno(EINVAL);
		return -1;
	}
	return __rt_sigprocmask(how, set, oldset, _NSIG / 8);
}


#else

#define __NR___syscall_sigprocmask __NR_sigprocmask
static inline
_syscall3(int, __syscall_sigprocmask, int, how, const sigset_t *, set,
		  sigset_t *, oldset);

int __sigprocmask(int how, const sigset_t * set, sigset_t * oldset)
{
#ifdef SIGCANCEL
	sigset_t local_newmask;

	/*
	 * The only thing we have to make sure here is that SIGCANCEL and
	 * SIGSETXID are not blocked.
	 */
	if (set != NULL && (__builtin_expect (__sigismember (set, SIGCANCEL), 0)
# ifdef SIGSETXID
		|| __builtin_expect (__sigismember (set, SIGSETXID), 0)
# endif
		))
	{
		local_newmask = *set;
		__sigdelset (&local_newmask, SIGCANCEL);
# ifdef SIGSETXID
		__sigdelset (&local_newmask, SIGSETXID);
# endif
		set = &local_newmask;
	}
#endif

	if (set &&
#if (SIG_BLOCK == 0) && (SIG_UNBLOCK == 1) && (SIG_SETMASK == 2)
		(((unsigned int) how) > 2)
#else
#warning "compile time assumption violated.. slow path..."
		((how != SIG_BLOCK) && (how != SIG_UNBLOCK)
		 && (how != SIG_SETMASK))
#endif
		) {
		__set_errno(EINVAL);
		return -1;
	}
	return (__syscall_sigprocmask(how, set, oldset));
}
#endif

weak_alias (__sigprocmask, sigprocmask)
