/* vi: set sw=4 ts=4: */
/*
 * select() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <sys/select.h>

#ifdef __UCLIBC_HAS_THREADS_NATIVE__
#include <sysdep-cancel.h>
#else
#define SINGLE_THREAD_P 1
#endif


libc_hidden_proto(select)

#ifdef __NR__newselect
# undef __NR_select
# define __NR_select __NR__newselect
#endif

#define __NR___syscall_select __NR_select
static inline _syscall5(int, __syscall_select, int, n, fd_set *, readfds,
		fd_set *, writefds, fd_set *, exceptfds, struct timeval *, timeout);

int select(int n, fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
		   struct timeval * timeout)
{
	if (SINGLE_THREAD_P)
		return __syscall_select(n, readfds, writefds, exceptfds, timeout);

#ifdef __UCLIBC_HAS_THREADS_NATIVE__
	int oldtype = LIBC_CANCEL_ASYNC ();
	int result = __syscall_select(n, readfds, writefds, exceptfds, timeout);
	LIBC_CANCEL_RESET (oldtype);
	return result;
#endif
}

libc_hidden_def(select)
