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

	.global	_start
	.global	__main
	.global _end
/*	.global __data_start */

	.bss
	.global environ
environ:
	.long 0

	.text
_start:			/* renamed from __start */
	nop
	nop

	movea.l %d5, %a5

	lea	__bss_start(%a5), %a0
	lea	end(%a5), %a1
 
	/* Copy 0 to %a0 until %a0 == %a1 */
	/*
	From my understanding of linux/fs/binfmt_flat.c for uClinux,
	this is not necessary anymore.  The loader will clear out
	the BSS for us. - jgraves@deltamobile.com

L1:
	movel   #0, %a0@+
	cmpal   %a0, %a1
	bhi     L1
	*/

	move.l 8(%sp), %d5
	move.l %d5, environ(%a5)
	
	bsr main

	move.l %d0,%sp@-
	bsr	exit		/* Invoke exit() routine */

#ifdef NO_LIBGCC
	/* If that didn't kill us, ... */
_exit:
	move.l %sp@+,%d1
	moveq #1,%d0 /* SYS_exit */
	trap #0

__main:
	rts
#else

	.global	_cleanup
_cleanup:
	rts	/* nothing to clean up */

#endif /* NO_LIBGCC */
