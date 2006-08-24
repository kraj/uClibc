/* vi: set sw=4 ts=4: */
/*
 * select() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <sys/select.h>

libc_hidden_proto(select)

#ifdef __NR__newselect
# undef __NR_select
# define __NR_select __NR__newselect
#endif
_syscall5(int, select, int, n, fd_set *, readfds, fd_set *, writefds,
		  fd_set *, exceptfds, struct timeval *, timeout);
libc_hidden_def(select)
