/* Copyright (C) 1991, 1992 Free Software Foundation, Inc.
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
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */


#define BASEREG r9

@ r0 = argc
@ r1 = argv
@ r2 = envp
@ sl = data segment



	.text
	.align 2
	.global _start, exit, main
	.type _start,%function

.text
_start:
    @ adjust the data sgement base pointer
	ldr r3,=__data_start
	sub sl,sl,r3
	mov	BASEREG,sl

	ldr	r3, .L3
	str r2,[BASEREG,r3]
	ldr r0,[sp, #0]
	ldr r1,[sp, #4]
	ldr r2,[sp, #8]
	bl  main
	ldr r0,=0
	bl  exit


.L3:  .word environ


	.data
	.align 2
	.global __environ
	
__environ:  .long 0
.weak environ
environ = __environ

