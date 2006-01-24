/* Initial C runtime code for Blackfin
 *
 * Copyright (C) 2004-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

/* When we enter this piece of code, the user stack looks like this:
*      argc            argument counter (integer)
*      argv[0]         program name (pointer)
*      argv[1...N]     program args (pointers)
*      NULL
*      env[0...N]      environment variables (pointers)
*      NULL

*   When we are done here, we want
*	R0=argc
*	R1=*argv[0]
*	R2=*envp[0]
*/

#include <features.h>

.text
.align 2
.global __start;
.type	__start,STT_FUNC;
.global ___uClibc_main;
.type	___uClibc_main,STT_FUNC;

#if defined(__UCLIBC_CTOR_DTOR__)
.type   __init,%function
.type   __fini,%function
#else
.weak   __init
.weak   __fini
#endif

/*
	When we enter, our stack looks like:
	[ SP ]
	[argc][argv]...

	Call ___uClibc_main(argc, argv, __init, __fini)
	                     R0    R1     R2    stack

	Before we call main, we want:
	[ SP ]
	[fini][argc][argv]
*/

__start:

/*	clear the frame pointer */
	FP = 0;

/*	Load register R0 (argc) from the stack to its final resting place */
	P0 = SP;
	R0 = [P0++];

/*	Copy argv pointer into R1 */
	R1 = P0;

/*	Load __init into R2 */
	R2 = __init;

/*	Load __fini onto the stack */
	P0 = __fini;
	[SP--] = P0;

/*	Ok, now run uClibc's main() -- shouldn't return */
	sp += -8;
	jump.l	___uClibc_main;
.size __start,.-__start
