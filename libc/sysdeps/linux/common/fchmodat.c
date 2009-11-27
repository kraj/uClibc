/*
 * fchmodat() for uClibc
 *
 * Copyright (C) 2009 Analog Devices Inc.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <sys/stat.h>

#ifdef __NR_fchmodat
_syscall4(int, fchmodat, int, fd, const char *, file, mode_t, mode, int, flag)
#else
/* should add emulation with fchmod() and /proc/self/fd/ ... */
#endif
