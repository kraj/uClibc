/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <features.h>
#include <sys/syscall.h>

#ifndef __NR_vfork
/* No vfork so use fork instead */
hidden_strong_alias(__libc_fork,__vfork)
#else
.text
.global __vfork
.hidden __vfork
.type   __vfork,%function
.align 1<<4

__vfork:
	popl %ecx
	movl $__NR_vfork,%eax
	int $0x80
	pushl %ecx
	cmpl $-4095,%eax
	jae __syscall_error
	ret

.size __vfork,.-__vfork
#endif
weak_alias(__vfork,vfork)
libc_hidden_def(vfork)
