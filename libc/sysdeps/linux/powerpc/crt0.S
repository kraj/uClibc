/*
 * uC-libc/sysdeps/linux/powerpc/crt0.S
 * process init code for powerpc
 *
 * Copyright (C) 2001 by Lineo, Inc.
 * Author: David A. Schleef <ds@schleef.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

/*
   _start is called with the following registers initialized:

     r1  null stack frame
     r3  argc
     r4  argv
     r5  envp
     r6  auxiliary vector.  ignored.
     r7  function pointer to jump to when main() exits.  If
         this is non-zero, it should be registered with atexit().
	 Linux always (?) sets r7 to NULL, so it is not implemented
	 here.
 */


.text
	.global	_start
	.global	__uClibc_main

	.type	_start,%function
	.type	__uClibc_main,%function

_start:
	/* create 2 empty stack frames */
	stwu	1,-32(1)

	lwz	3,32(1)
	addi	4,1,36
	addi	5,1,40
	rlwinm	0,3,2,0,29
	add	5,5,0

	bl	__uClibc_main
	
/* a little bit of stuff to support C++ */
	.section .ctors,"aw"
	.align 4
	.global __CTOR_LIST__
__CTOR_LIST__:
	.long -1

	.section .dtors,"aw"
	.align 4
	.global __DTOR_LIST__
__DTOR_LIST__:
	.long -1

