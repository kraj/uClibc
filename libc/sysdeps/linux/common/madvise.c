/* vi: set sw=4 ts=4: */
/*
 * madvise() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <sys/mman.h>
#ifdef __NR_madvise
_syscall3(int, madvise, void *, __addr, size_t, __len, int, __advice);
#endif
