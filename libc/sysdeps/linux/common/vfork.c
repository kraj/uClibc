/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

/* Trivial implementation for arches that lack vfork */
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#ifdef __NR_fork
libc_hidden_proto(fork)

pid_t attribute_hidden __vfork(void);
pid_t attribute_hidden __vfork(void)
{
    return fork();
}
libc_hidden_proto(vfork)
weak_alias(__vfork,vfork)
libc_hidden_def(vfork)
#endif
