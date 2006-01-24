/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */
#include <features.h>
#include <sys/syscall.h>

#ifndef __NR_vfork

/* No vfork so use fork instead */
strong_alias(fork,__libc_fork)
hidden_strong_alias(__libc_fork,__vfork)

#else

.text
.global __vfork
.hidden __vfork
.type   __vfork,@function
.type   __syscall_error,@function

__vfork:
	li	0, __NR_vfork
	sc
	bnslr+
	b	__syscall_error

.size __vfork,.-__vfork
#endif
weak_alias(__vfork,vfork)
libc_hidden_def(vfork)
