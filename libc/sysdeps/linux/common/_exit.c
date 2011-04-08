/* vi: set sw=4 ts=4: */
/*
 * exit syscall for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>

void _exit(int status)
{
	/* The loop is added only to keep gcc happy. */
	while(1)
	{
#if defined __NR_exit_group && defined __UCLIBC_HAS_THREADS_NATIVE__
		INLINE_SYSCALL(exit_group, 1, status);
#endif
		INLINE_SYSCALL(exit, 1, status);
	}
}
libc_hidden_def(_exit)
weak_alias(_exit,_Exit)
