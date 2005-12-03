/* vi: set sw=4 ts=4: */
/*
 * times() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include "syscalls.h"
#include <sys/times.h>
#define __NR___times __NR_times
attribute_hidden _syscall1(clock_t, __times, struct tms *, buf);
strong_alias(__times,times)
