/* setjmp for ARM.
   Copyright (C) 1997, 1998 Free Software Foundation, Inc.
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

#include <features.h>
#define _SETJMP_H
#define _ASM
#include <bits/setjmp.h>

.global __sigsetjmp
.type __sigsetjmp,%function
.align 4
__sigsetjmp:
	/* Save registers */
#if defined __UCLIBC_HAS_FLOATS__ && ! defined __UCLIBC_HAS_SOFT_FLOAT__
# ifdef __MAVERICK__
	cfstrd	mvd4,  [r0], #8 ; nop
	cfstrd	mvd5,  [r0], #8 ; nop
	cfstrd	mvd6,  [r0], #8 ; nop
	cfstrd	mvd7,  [r0], #8 ; nop
	cfstrd	mvd8,  [r0], #8 ; nop
	cfstrd	mvd9,  [r0], #8 ; nop
	cfstrd	mvd10, [r0], #8 ; nop
	cfstrd	mvd11, [r0], #8 ; nop
	cfstrd	mvd12, [r0], #8 ; nop
	cfstrd	mvd13, [r0], #8 ; nop
	cfstrd	mvd14, [r0], #8 ; nop
	cfstrd	mvd15, [r0], #8
# else
	sfmea   f4, 4, [r0]!
# endif
#else
# ifdef __MAVERICK__
	add     r0, r0, #96		/* skip the FP registers */
# else
	add     r0, r0, #48		/* skip the FP registers */
# endif
#endif
	stmia   r0, {v1-v6, sl, fp, sp, lr}

	/* Restore pointer to jmp_buf */
#ifdef __MAVERICK__
	sub     r0, r0, #96
#else
	sub     r0, r0, #48
#endif

	/* Make a tail call to __sigjmp_save; it takes the same args.  */
#ifdef __PIC__
	B	__sigjmp_save(PLT)
#else
	B	__sigjmp_save
#endif

.size __sigsetjmp,.-__sigsetjmp
