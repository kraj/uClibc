/* vi: set sw=4 ts=4: */
/*
 * msync() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <unistd.h>

#if defined __NR_msync && defined __ARCH_USE_MMU__

#include <sys/mman.h>

#ifdef __LINUXTHREADS_OLD__
extern __typeof(msync) weak_function msync;
strong_alias(msync,__libc_msync)
#endif

_syscall3(int, msync, void *, addr, size_t, length, int, flags)

#endif
