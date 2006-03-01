/* Initial C runtime code for Blackfin
 *
 * Copyright (C) 2004-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

/* When we enter this piece of code, the user stack looks like this:
* [SP] argc            argument counter (integer)
*      argv[0]         program name (pointer)
*      argv[1...N]     program args (pointers)
*      NULL
*      env[0...N]      environment variables (pointers)
*      NULL

*   When we are done here, we want
*	R0=argc
*	R1=argv
*	R2=__init
*   SP=__fini
*/

#include <features.h>

.text
.align 2
.global __start;
.type	__start,STT_FUNC;
.global ___uClibc_main;
.type	___uClibc_main,STT_FUNC;

#define __UCLIBC_CTOR_DTOR__
#if defined(__UCLIBC_CTOR_DTOR__)
.weak	__init;
.weak	__fini;
#endif

__start:

/*	clear the frame pointer and the L registers.  */
	FP = 0;
	L0 = 0;
	L1 = 0;
	L2 = 0;
	L3 = 0;

/*	Load register R0 (argc) from the stack to its final resting place */
	P0 = SP;
	R0 = [P0++];

/*	Copy argv pointer into R1 */
	R1 = P0;

#if defined(__UCLIBC_CTOR_DTOR__)
/*	Load __init into R2 */
	R2.H = __init;
	R2.L = __init;

/*	Load __fini onto the stack */
	SP += -16;
	R3.H = __fini;
	R3.L = __fini;
	[SP+12] = R3;
#else
/*	Just fixup the stack */
	sp += -12;
#endif

/*	Ok, now run uClibc's main() -- shouldn't return */
	jump.l	___uClibc_main;

.size __start,.-__start
