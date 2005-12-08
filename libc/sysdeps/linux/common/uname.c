/* vi: set sw=4 ts=4: */
/*
 * uname() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <sys/utsname.h>
#define __NR___uname __NR_uname
attribute_hidden _syscall1(int, __uname, struct utsname *, buf);
strong_alias(__uname,uname)
