/* vi: set sw=4 ts=4: */
/* vfork for uClibc
 *
 * Copyright (C) 2000 by Lineo, inc. and Erik Andersen
 * Copyright (C) 2000,2001 by Erik Andersen <andersen@uclibc.org>
 * Written by Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <features.h>

#define _ERRNO_H
#include <bits/errno.h>
#include <sys/syscall.h>

#ifdef __NR_fork
.text
.global	__vfork
.hidden	__vfork
.type	__vfork,%function
.align 4

__vfork:

#ifdef __NR_vfork
	swi	__NR_vfork
	cmn	r0, #4096
	movcc	pc, lr

	/* Check if vfork even exists.  */
	ldr     r1, =-ENOSYS
	teq     r0, r1
	bne     __error
#endif

	/* If we don't have vfork, use fork.  */
	swi     __NR_fork
	cmn     r0, #4096

	/* Syscall worked.  Return to child/parent */
	movcc   pc, lr

__error:
	b	__syscall_error

.size __vfork,.-__vfork
strong_alias(__vfork,vfork)
#endif
