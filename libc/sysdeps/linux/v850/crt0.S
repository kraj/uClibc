/*
 * libc/sysdeps/linux/v850/crt0.S -- Initial program entry point for linux/v850
 *
 *  Copyright (C) 2001,2002  NEC Corporation
 *  Copyright (C) 2001,2002  Miles Bader <miles@gnu.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License.  See the file COPYING.LIB in the main
 * directory of this archive for more details.
 * 
 * Written by Miles Bader <miles@gnu.org>
 */

#include <clinkage.h>

/* Upon entry, the stack contains the following data:
	argc, argv[0], ..., argv[argc-1], 0, envp[0], ..., 0
*/

	.text
C_ENTRY(start):	
	ld.w	0[sp], r6		// Arg 0: argc

	addi	4, sp, r7		// Arg 1: argv

	mov	r7, r8			// Arg 2: envp
	mov	r6, r10			// skip argc elements to get envp start
	add	1, r10			// ...plus the NULL at the end of argv
	shl	2, r10			// Convert to byte-count to skip
	add	r10, r8

	// Zero bss area, since we can't rely upon any loader to do so
	mov	hilo(C_SYMBOL_NAME(edata)), ep
	mov	hilo(C_SYMBOL_NAME(end)), r10
2:	cmp	ep, r10
	be	3f
	sst.w	r0, 0[ep]
	add	4, ep
	br	2b

	// Load CTBP register
3:	mov	hilo(C_SYMBOL_NAME(_ctbp)), r19
	ldsr	r19, ctbp

	// Load GP
	mov	hilo(C_SYMBOL_NAME(_gp)), gp

	// tail-call uclibc's startup routine
	jr	C_SYMBOL_NAME(__uClibc_main)


/* Stick in a dummy reference to `main', so that if an application
   is linking when the `main' function is in a static library (.a)
   we can be sure that `main' actually gets linked in.  */
L_dummy_main_reference:
	.long	C_SYMBOL_NAME(main)
