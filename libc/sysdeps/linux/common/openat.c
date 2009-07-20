/*
 * openat() for uClibc
 *
 * Copyright (C) 2009 Analog Devices Inc.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define openat __xx_openat
#include <sys/syscall.h>
#include <fcntl.h>
#undef openat

#ifdef __NR_openat
_syscall4(int, openat, int, fd, const char *, file, int, oflag, mode_t, mode)
#else
/* should add emulation with open() and /proc/self/fd/ ... */
#endif
