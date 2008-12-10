/* vi: set sw=4 ts=4: */
/*
 * pause() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define __UCLIBC_HIDE_DEPRECATED__
#include <sys/syscall.h>
#include <unistd.h>

#ifdef __NR_pause

_syscall0(int, pause)

#else

#include <signal.h>
int pause(void)
{
	return (__sigpause(sigblock(0), 0));
}

#endif
