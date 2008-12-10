/* vi: set sw=4 ts=4: */
/*
 * writev() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <sys/uio.h>

_syscall3(ssize_t, writev, int, filedes, const struct iovec *, vector,
		  int, count)
