/* vi: set sw=4 ts=4: */
/*
 * select() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <unistd.h>

#ifdef __NR__newselect
#define __NR___select __NR__newselect
#else
#define __NR___select __NR_select
#endif
attribute_hidden _syscall5(int, __select, int, n, fd_set *, readfds, fd_set *, writefds,
		  fd_set *, exceptfds, struct timeval *, timeout);
strong_alias(__select,select)
