/* Startup code for Alpha/ELF.
   Copyright (C) 1993,1995,1996,1997,1998,2000,2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Richard Henderson <rth@tamu.edu>

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


	.text
	.align 3
	.globl _start
	.ent _start, 0
	.type _start,@function
_start:
	.frame $15, 0, $31
	mov	$31, $15
	br	$29, 1f
1:	ldgp	$29, 0($29)
	subq	$30, 16, $30
	.prologue 0

  /* Load address of the user's main function.  */
	lda	$16, main

	ldl	$17, 16($30)	/* get argc */
	lda	$18, 24($30)	/* get argv */

#if 0
  /* Load address of our own entry points to .fini and .init.  */
	lda	$19, _init
	lda	$20, _fini

  /* Store address of the shared library termination function.  */
	mov	$0, $21

  /* Provide the highest stack address to the user code.  */
	stq	$30, 0($30)
#endif

  /* Call the user's main function, and exit with its value.
     But let the libc call main.  */
	jsr	$26, __uClibc_main

  /* Die very horribly if exit returns.  Call_pal hlt is callable from
     kernel mode only; this will result in an illegal instruction trap.  */
	call_pal 0
	.end _start

/* For ECOFF backwards compatibility. */
.weak    __start;
    __start    =   _start

/* Define a symbol for the first piece of initialized data.  */
	.data
	.globl __data_start
__data_start:
	.long 0
	.weak data_start
	data_start = __data_start
