/* Copyright (C) 1999, 2000 Free Software Foundation, Inc.
   Copyright (C) 2001 Hewlett-Packard Australia

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Library General Public License as published by the Free
 Software Foundation; either version 2 of the License, or (at your option) any
 later version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for more
 details.

 You should have received a copy of the GNU Library General Public License
 along with this program; if not, write to the Free Software Foundation, Inc.,
 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

 Derived in part from the Linux-8086 C library, the GNU C Library, and several
 other sundry sources.  Files within this library are copyright by their
 respective copyright holders.
*/

#include <sysdep.h>
#define _ERRNO_H	1
#include <bits/errno.h>

/* Clone the calling process, but without copying the whole address space.
   The calling process is suspended until the new process exits or is
   replaced by a call to `execve'.  Return -1 for errors, 0 to the new process,
   and the process ID of the new process to the old process.  */

.global errno

ENTRY (__vfork)

	mov.l	@r15+,r3		// pop value from the stack
	mov.l	.L5,r1
	mov.l	r3,@r1			// save it in .sav_stack

	mov.w	.L3, r3

#ifdef HIOS
	trapa	#0x28
#else
	trapa	#0x10
#endif

	mov     r0, r1
// 12 arithmetic shifts for the crappy sh2, because shad doesn't exist!	
	shar	r1
	shar	r1
	shar	r1
	shar	r1
	shar	r1
	shar	r1
	shar	r1
	shar	r1
	shar	r1
	shar	r1
	shar	r1
	shar	r1

//	mov	#-12, r2
//	shad	r2, r1
	not	r1, r1			// r1=0 means r0 = -1 to -4095
	tst	r1, r1			// i.e. error in linux
	bf	1f
	mov.w	.L1, r1
	cmp/eq	r1, r0
	bt	2f
	mov.l	.L2, r1
	jmp	@r1
	 mov	r0, r4

	.align	4

1:
	mov.l	.L5,r1
	mov.l	@r1,r3			// get it from .sav_stack
	mov.l	r3,@-r15		// restore value to the stack

	rts
	 nop

	.align	4

2:
__syscall_error:
	/* Store it in errno... */
	mov.l	.L4, r1
	mov.l	r0, @r1

	mov.l	.L5,r1
	mov.l	@r1,r3			// get it from .sav_stack
	mov.l	r3,@-r15		// restore value to the stack

	/* And just kick back a -1.  */
	rts
	 mov	#-1, r0

	.align	4
.L1:
	.word	-ENOSYS
.L3:
	.word	190			//__NR_vfork

	.align	4			// Shouldn't be necessary as previously with have two words
.L2:
	.long	__syscall_error

.L4:	.long	errno

.L5:	.long	.sav_stack

	.data

	.align 4

.sav_stack:					//area to temporary save the stach
	.long	0

.weak	vfork
vfork = __vfork
