/* vi: set sw=4 ts=4: */
/*
 * mremap() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "syscalls.h"
#include <unistd.h>
#include <sys/mman.h>

libc_hidden_proto(mremap)

_syscall4(__ptr_t, mremap, __ptr_t, old_address, size_t, old_size, size_t,
		  new_size, int, may_move);
libc_hidden_def(mremap)
