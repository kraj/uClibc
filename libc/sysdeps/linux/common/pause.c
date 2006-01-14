/* vi: set sw=4 ts=4: */
/*
 * pause() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <unistd.h>

#ifdef __NR_pause
#define __NR___libc_pause __NR_pause
_syscall0(int, __libc_pause);
#else
#include <signal.h>
libc_hidden_proto(__sigpause)
libc_hidden_proto(sigblock)

int __libc_pause(void)
{
	return (__sigpause(sigblock(0), 0));
}
#endif
strong_alias(__libc_pause,pause)
