/* vi: set sw=4 ts=4: */
/*
 * Copyright (C) 2006 Steven J. Hill <sjhill@realitydiluted.com>
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#ifdef __UCLIBC_HAS_THREADS_NATIVE__
#include <errno.h>
#include <sysdep-cancel.h>
#else

libc_hidden_proto(wait4)
#endif

extern __typeof(waitpid) __libc_waitpid;
__pid_t __libc_waitpid(__pid_t pid, int *wait_stat, int options)
{
#ifdef __UCLIBC_HAS_THREADS_NATIVE__
	if (SINGLE_THREAD_P)
	{
#ifdef __NR_waitpid
		return INLINE_SYSCALL (waitpid, 3, pid, wait_stat, options);
#else
		return INLINE_SYSCALL (wait4, 4, pid, wait_stat, options, NULL);
#endif
	}

	int oldtype = LIBC_CANCEL_ASYNC ();

#ifdef __NR_waitpid
	int result = INLINE_SYSCALL (waitpid, 3, pid, wait_stat, options);
#else
	int result = INLINE_SYSCALL (wait4, 4, pid, wait_stat, options, NULL);
#endif

	LIBC_CANCEL_RESET (oldtype);

	return result;
}
#else
	return wait4(pid, wait_stat, options, NULL);
}
#endif
libc_hidden_proto(waitpid)
weak_alias(__libc_waitpid,waitpid)
libc_hidden_weak(waitpid)
