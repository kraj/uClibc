/* Copyright (C) 1991 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

/* Hacked up for uClibc by Erik Andersen */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

typedef void (*vfuncp) ();
extern vfuncp __cleanup;
extern void _exit __P((int __status)) __attribute__ ((__noreturn__));

/* Cause an abnormal program termination with core-dump.  */
void abort(void)
{
#if FIXME
	sigset_t sigset;

	if (sigemptyset(&sigset) == 0 && sigaddset(&sigset, SIGABRT) == 0) {
		sigprocmask(SIG_UNBLOCK, &sigset, (sigset_t *) NULL);
	}
#endif

	if (__cleanup)
		__cleanup();

	while (1)
		if (raise(SIGABRT))
			/* If we can't signal ourselves, exit.  */
			_exit(127);
	/* If we signal ourselves and are still alive,
	   or can't exit, loop forever.  */
}
