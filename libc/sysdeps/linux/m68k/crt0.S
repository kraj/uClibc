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

/*
 * NOTE: this file works for PIC and non-PIC code.  Be very careful how
 * you modify it !
 */
	.global	_start
	.global	__main
	.global _end
	.global _sdata

	.bss
	.global environ
environ:
	.long 0

	.text
_start:			/* renamed from __start */
	nop
	nop

	movea.l %d5, %a5	/* uClinux passes in data segment here */

	move.l 8(%sp), %d5

	lea.l _sdata, %a1	/* set environ to point to the right place */
	lea.l environ, %a0
	sub.l %a1, %a0
	move.l %d5, %a5@(%a0)
	
	lea main-.-8, %a0	/* call main */
	jsr %pc@(%a0)

	move.l %d0,%sp@-

	lea exit-.-8, %a0	/* call the exit routine */
	jsr	%pc@(%a0)

#ifdef NO_LIBGCC
	/* If that didn't kill us, ... */
_exit:
	move.l %sp@+,%d1
	moveq #1,%d0 /* SYS_exit */
	trap #0

__main:
	rts
#else

#endif /* NO_LIBGCC */
