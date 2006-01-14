/* vi: set sw=4 ts=4: */
/*
 * readlink() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>

libc_hidden_proto(readlink)

_syscall3(int, readlink, const char *, path, char *, buf, size_t, bufsiz);
libc_hidden_def(readlink)
