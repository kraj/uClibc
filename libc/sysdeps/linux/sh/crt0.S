/* Startup code for SH & ELF.
   Copyright (C) 1999 Free Software Foundation, Inc.
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

/* This is the canonical entry point, usually the first thing in the text
   segment.

	At this entry point, most registers' values are unspecified, except:

   r4		Contains a function pointer to be registered with `atexit'.
   		This is how the dynamic linker arranges to have DT_FINI
		functions called for shared libraries that have been loaded
		before this code runs.
		WARNING: At that stage only static linker is supported. For
		uCLinux we won't bother with r4.

   sp		The stack contains the arguments and environment:
   		0(sp)			argc
		4(sp)			argv[0]
		...
		(4*argc)(sp)		NULL
		(4*(argc+1))(sp)	envp[0]
		...
					NULL
*/

	.text
	.globl _start
	
_start:
	/* Clear the frame pointer since this is the outermost frame.  */
	mov #0, r14

	/* Pop argc off the stack and save a pointer to argv */
	mov.l @r15+,r4
	mov r15, r5

	/* set up the value for the environment pointer
	   r6 = (argc+1)*4+argv
	*/
	mov r4,r6
	add #1,r6
	shll2 r6
	add r5,r6

	!			Clear BSS area
	mov.l	3f, r1
	add	#4, r1
	mov.l	4f, r2
	mov	#0, r0
9:	cmp/hs	r2, r1
	bf/s	9b		! while (r1 < r2)
	 mov.l	r0,@-r2

	/* call main */
	mov.l L_main,r1
	jsr @r1
	nop

	/* should never get here....*/
	mov.l L_abort,r1
	jsr @r1
	nop

	.align	2

3:	.long	__bss_start
4:	.long	_end

L_main:
	.long	__uClibc_main

L_abort:
	.long	abort

	.data
