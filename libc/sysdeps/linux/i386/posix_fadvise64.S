/* Copyright (C) 1995-2000,2002,2003,2004,2005,2006
      Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#define _ERRNO_H	1
#include <features.h>
#include <bits/errno.h>
#include <sys/syscall.h>

.text
.global posix_fadvise64
.type   posix_fadvise64,%function
posix_fadvise64:
#ifdef __NR_posix_fadvise64_64

	/* Save regs  */
	pushl	%ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi

	movl $__NR_posix_fadvise64_64, %eax	/* Syscall number in %eax.  */

	movl	24(%esp), %ebx
	movl	28(%esp), %ecx
	movl	32(%esp), %edx
	movl	36(%esp), %esi
	movl	40(%esp), %edi
	movl	44(%esp), %ebp

	/* Do the system call trap.  */
	int $0x80

	/* Restore regs  */
	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp

	/* If 0 > %eax > -4096 there was an error.  */
	cmpl $-4096, %eax
	ja __syscall_error
#else
	movl	$-ENOSYS, %eax
	jmp __syscall_error
#endif
	/* Successful; return the syscall's value.  */
	ret

.size posix_fadvise64,.-posix_fadvise64

libc_hidden_def(posix_fadvise64)

