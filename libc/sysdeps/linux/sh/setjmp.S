/* setjmp for SH3.
   Copyright (C) 1999, 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <sysdep.h>

#define _SETJMP_H
#define _ASM
#include <bits/setjmp.h>

ENTRY (__sigsetjmp)
/*
	.globl	__sigsetjmp
	.align	4
__sigsetjmp:
*/
	/* Save registers */
	add	#(JB_SIZE - 4 * 5), r4
	stc.l	gbr, @-r4
	sts.l	pr, @-r4
	mov.l	r15, @-r4
	mov.l	r14, @-r4
	mov.l	r13, @-r4
	mov.l	r12, @-r4
	mov.l	r11, @-r4
	mov.l	r10, @-r4
	mov.l	r9, @-r4
	mov.l	r8, @-r4

	/* Make a tail call to __sigjmp_save; it takes the same args.  */
	mov.l	.L1, r1
	jmp	@r1
	 nop
	.align	2
.L1:
	.long	__sigjmp_save
END (__sigsetjmp)
