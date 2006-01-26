/* vi: set sw=4 ts=4: */
/*
 * ioperm() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#if defined __ARCH_HAS_MMU__ && defined __NR_ioperm
#include <sys/perm.h>
_syscall3(int, ioperm, unsigned long, from, unsigned long, num, int, turn_on);
#endif
