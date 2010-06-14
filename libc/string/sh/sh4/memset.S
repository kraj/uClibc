/* $Id: memset.S,v 1.1 2000/04/14 16:49:01 mjd Exp $
 *
 * "memset" implementation of SuperH
 *
 * Copyright (C) 1999  Niibe Yutaka
 *
 * Copyright (c) 2009  STMicroelectronics Ltd
 *   Optimised using 64bit data transfer via FPU
 *   Author: Giuseppe Cavallaro <peppe.cavallaro@st.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

/*
 *            void *memset(void *s, int c, size_t n);
 */

#include <sysdep.h>

#if defined (__LITTLE_ENDIAN__) && defined (__SH_FPU_ANY__)
#define MEMSET_USES_FPU
/* Use paired single precision load or store mode for 64-bit tranfering.
 * FPSCR.SZ=1,FPSCR.SZ=0 is well defined on both SH4-200 and SH4-300.
 * Currenlty it has been only implemented and tested for little endian mode. */
.macro FPU_SET_PAIRED_PREC
	sts	fpscr, r3
	mov	#0x10, r0	! PR=0 SZ=1
	shll16  r0
	lds	r0, fpscr
.endm
.macro RESTORE_FPSCR
	lds	r3, fpscr
.endm
#endif

ENTRY(memset)
	tst	r6,r6
	bt/s	5f		! if n=0, do nothing
	 add	r6,r4
	mov	#12,r0
	cmp/gt	r6,r0
	bt/s	4f		! if it's too small, set a byte at once
	 mov	r4,r0
	and	#3,r0
	cmp/eq	#0,r0
	bt/s	2f		! It's aligned
	 sub	r0,r6
1:
	dt	r0
	bf/s	1b
	 mov.b	r5,@-r4
2:				! make VVVV
	extu.b	r5,r5
	swap.b	r5,r0		!   V0
	or	r0,r5		!   VV
	swap.w	r5,r0		! VV00
	or	r0,r5		! VVVV

	! Enough bytes need to be copied
	mov	#0x40, r0	! (MT)
	cmp/gt	r6,r0		! (MT)  64 > len => slow loop

	bt/s	22f
	 mov	r6,r0

	! align the dst to the cache block size if necessary
	mov	r4, r3
	mov	#~(0x1f), r1

	and	r3, r1
	cmp/eq	r3, r1

	bt/s	11f		! dst is already aligned
	 sub	r1, r3		! r3-r1 -> r3
	shlr2	r3		! number of loops

10:	mov.l	r5,@-r4
	dt	r3
	bf/s	10b
	 add	#-4, r6

11:	! dst is 32byte aligned
	mov	r6,r2
	mov	#-5,r0
	shld	r0,r2		! number of loops

#ifdef MEMSET_USES_FPU
	lds	r5, fpul	! (CO)
	fsts	fpul, fr0	! Dr0 will be 'VVVVVVVV'
	fsts	fpul, fr1

	FPU_SET_PAIRED_PREC
12:
	add	#-0x20, r6	!(MT)
	fmov	dr0, @-r4
	fmov	dr0, @-r4
	fmov	dr0, @-r4
	dt	r2
	bf/s	12b		!(BR)
	 fmov	dr0, @-r4

	RESTORE_FPSCR
#else
12:
	mov.l	r5,@-r4
	mov.l	r5,@-r4
	mov.l	r5,@-r4
	mov.l	r5,@-r4
	mov.l	r5,@-r4
	mov.l	r5,@-r4
	add	#-0x20, r6
	mov.l	r5,@-r4
	dt	r2
	bf/s	12b
	 mov.l	r5,@-r4
#endif
	tst	r6,r6
	bt/s	5f
	 mov	#8, r0

	cmp/ge	r0, r6
	bf/s	4f
	 mov	r6,r0
22:
	shlr2	r0
	shlr	r0		! r0 = r6 >> 3
3:
	dt	r0
	mov.l	r5,@-r4		! set 8-byte at once
	bf/s	3b
	 mov.l	r5,@-r4
	!
	mov	#7,r0
	and	r0,r6
	tst	r6,r6
	bt	5f
	! fill bytes
4:
	dt	r6
	bf/s	4b
	 mov.b	r5,@-r4
5:
	rts
	 mov	r4,r0
END(memset)
libc_hidden_def (memset)
