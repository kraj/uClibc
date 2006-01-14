/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>

libc_hidden_proto(vfork)

_syscall0(pid_t, vfork);
libc_hidden_def(vfork)
