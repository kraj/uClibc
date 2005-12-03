/* vi: set sw=4 ts=4: */
/*
 * getpid() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <unistd.h>

#if defined (__alpha__)
#define __NR___getpid __NR_getxpid
#endif
#define __NR___getpid __NR_getpid
attribute_hidden _syscall0(pid_t, __getpid);
strong_alias(__getpid, getpid)
/* not used in libpthread */
/* weak_alias(__getpid, __libc_getpid) */
