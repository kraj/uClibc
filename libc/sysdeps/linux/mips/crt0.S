/* When we enter this piece of code, the program stack looks like this:
        argc            argument counter (integer)
        argv[0]         program name (pointer)
        argv[1...N]     program args (pointers)
        argv[argc-1]    end of args (integer)
	NULL
        env[0...N]      environment variables (pointers)
        NULL
*/

#include <features.h>
#include "regdef.h"

.text
.global __start
.type __start,@function

__start:              
#ifdef __PIC__
        .set noreorder
        bltzal zero,0f
        nop
0:      .cpload $31
        .set reorder
#endif

	move $31, zero
	lw   a0, 0($29)	    /* argc */
	addu a1, $29, 4	    /* argv  */
	addu a2, a0, 1	    /* load envp */
        sll  a2, a2, 2
	add  a2, a2, a1
	/* Ok, now run uClibc's main() -- shouldn't return */
	jal	__uClibc_main
	hlt:    b hlt                   /* Crash if somehow it does return.  */

#if 0 /* this should be provided by crtbegin/crtend in the compiler */
/* a little bit of stuff to support C++ */
	.section .ctors,"aw"
	.align 4
	.global __CTOR_LIST__
__CTOR_LIST__:
	.long -1

	.section .dtors,"aw"
	.align 4
	.global __DTOR_LIST__
__DTOR_LIST__:
	.long -1
#endif
