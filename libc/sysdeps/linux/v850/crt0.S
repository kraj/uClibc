/*
 * libc/sysdeps/linux/v850/crt0.S -- Initial program entry point for linux/v850
 *
 *  Copyright (C) 2001  NEC Corporation
 *  Copyright (C) 2001  Miles Bader <miles@gnu.org>
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
1:	add	4, r8
	add	-1, r10
	bp	1b

	// Zero bss area, since we can't rely upon any loader to do so
	mov	hilo(C_SYMBOL_NAME(edata)), ep
	mov	hilo(C_SYMBOL_NAME(end)), r10
2:	sst.w	r0, 0[ep]
	add	4, ep
	cmp	ep, r10
	bne	2b

	// Load CTBP register
	mov	hilo(C_SYMBOL_NAME(_ctbp)), r19
	ldsr	r19, ctbp

	// Load GP
	mov	hilo(C_SYMBOL_NAME(_gp)), gp

	// call uclibc's startup routine
	jarl	C_SYMBOL_NAME(__uClibc_main), lp

	// should never get here....
	jr	C_SYMBOL_NAME(abort)
