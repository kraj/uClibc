/* Copyright (C) 1996, 1997,98,99,2000,02 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Richard Henderson (rth@tamu.edu)

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

/* clone() is even more special than fork() as it mucks with stacks
   and invokes a function in the right context after its all over.  
   
   Hacked up for uClibc by Erik Andersen <andersen@codepoet.org>
*/

#define _ERRNO_H	1
#include <bits/errno.h>
#include <sys/syscall.h>

#define LINKAGE     4
#define PTR_SIZE    4
#define PARMS       LINKAGE        /* no space for saved regs */
#define FUNC        PARMS
#define STACK       FUNC+4
#define FLAGS       STACK+PTR_SIZE
#define ARG         FLAGS+4
#define PTID        ARG+PTR_SIZE
#define TLS         PTID+PTR_SIZE
#define CTID        TLS+PTR_SIZE


        .text
	.globl __clone;
	.type __clone,@function;
	.align 1<<4;

__clone:
	/* Sanity check arguments.  */
	movl	$-EINVAL,%eax
	movl	FUNC(%esp),%ecx		/* no NULL function pointers */
#ifdef __PIC__
	jecxz	__syscall_error
#else
	testl	%ecx,%ecx
	jz	__syscall_error
#endif
	movl	STACK(%esp),%ecx	/* no NULL stack pointers */
#ifdef __PIC__
	jecxz	__syscall_error
#else
	testl	%ecx,%ecx
	jz	__syscall_error
#endif

	/* Insert the argument onto the new stack.  */
	subl	$16,%ecx
	movl	ARG(%esp),%eax		/* no negative argument counts */
	movl	%eax,12(%ecx)

	/* Save the function pointer as the zeroth argument.
	   It will be popped off in the child in the ebx frobbing below.  */
	movl	FUNC(%esp),%eax
	movl	%eax,8(%ecx)
	/* Don't leak any information.  */
	movl	$0,4(%ecx)
	movl	$0,(%ecx)

	/* Do the system call */
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	movl	TLS+12(%esp),%esi
	movl	PTID+12(%esp),%edx
	movl	FLAGS+12(%esp),%ebx
	movl	CTID+12(%esp),%edi
	movl	$__NR_clone,%eax
	int	$0x80
	popl	%edi
	popl	%esi
	popl	%ebx

	test	%eax,%eax
	jl	__syscall_error
	jz	.Lthread_start

.Lpseudo_end:
	ret

.Lthread_start:
	subl	%ebp,%ebp	/* terminate the stack frame */
	call	*%ebx
#ifdef __PIC__
	call	.Lhere
.Lhere:
	popl	%ebx
	addl	$_GLOBAL_OFFSET_TABLE_+[.-.Lhere], %ebx
#endif
	movl	%eax, %ebx
	movl	$__NR_exit, %eax
	int	$0x80

__syscall_error:
	negl    %eax
	pushl   %eax
#ifdef __PIC__
	call .Lthere
.Lthere:
	popl	%ebx
	addl	$_GLOBAL_OFFSET_TABLE_+[.- .Lthere  ], %ebx
	call    __errno_location@PLT
#else
	call	__errno_location
#endif
	popl	%ecx
	movl	%ecx, (%eax)
	xorl	%eax, %eax
	decl	%eax

.Lsize:
	.size	 __clone,.Lsize-__clone

.weak clone ; clone = __clone
