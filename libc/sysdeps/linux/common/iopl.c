/* vi: set sw=4 ts=4: */
/*
 * iopl() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
/* Tuns out the m68k unistd.h kernel header is broken */
#if defined __ARCH_HAS_MMU__ && defined __NR_iopl && ( !defined(__mc68000__))
#include <sys/perm.h>
_syscall1(int, iopl, int, level);
#endif
