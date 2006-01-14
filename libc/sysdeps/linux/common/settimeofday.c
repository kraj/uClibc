/* vi: set sw=4 ts=4: */
/*
 * settimeofday() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <sys/time.h>
attribute_hidden _syscall2(int, settimeofday, const struct timeval *, tv,
		  const struct timezone *, tz);
libc_hidden_proto(settimeofday)
libc_hidden_def(settimeofday)
