/* When we enter this piece of code, the program stack looks like this:
        argc            argument counter (integer)
        argv[0]         program name (pointer)
        argv[1...N]     program args (pointers)
        argv[argc-1]    end of args (integer)
	NULL
        env[0...N]      environment variables (pointers)
        NULL

   For uClinux it looks like this:

        argc            argument counter (integer)
        argv            char *argv[]
        envp            char *envp[]
        argv[0]         program name (pointer)
        argv[1...N]     program args (pointers)
        argv[argc-1]    end of args (integer)
	NULL
        env[0...N]      environment variables (pointers)
        NULL

   When we are done here, we want
	a1=argc
	a2=argv[0]
	a3=argv[argc+1]

ARM register quick reference:

    Name    Number       ARM Procedure Calling Standard Role

    a1      r0           argument 1 / integer result / scratch register / argc
    a2      r1           argument 2 / scratch register / argv
    a3      r2           argument 3 / scratch register / envp
    a4      r3           argument 4 / scratch register
    v1      r4           register variable
    v2      r5           register variable
    v3      r6           register variable
    v4      r7           register variable
    v5      r8           register variable
    sb/v6   r9           static base / register variable
    sl/v7   r10          stack limit / stack chunk handle / reg. variable
    fp      r11          frame pointer
    ip      r12          scratch register / new-sb in inter-link-unit calls
    sp      r13          lower end of current stack frame
    lr      r14          link address / scratch register
    pc      r15          program counter
*/

#include <features.h>

.text
	.global _start
	.type	_start,%function
#if defined L_crt0 || ! defined __UCLIBC_CTOR_DTOR__
	.type	__uClibc_main,%function
#else
	.weak	_init
	.weak	_fini
	.type	__uClibc_start_main,%function
#endif
/* Stick in a dummy reference to main(), so that if an application
 * is linking when the main() function is in a static library (.a)
 * we can be sure that main() actually gets linked in */
	.type	main,%function


.text
_start:
	/* clear the frame pointer */
	mov     fp, #0

#ifdef __ARCH_HAS_MMU__
	/* Load register r0 (argc) from the stack to its final resting place */
	ldr     r0, [sp], #4

	/* Copy argv pointer into r1 -- which its final resting place */
	mov     r1, sp

	/* Skip to the end of argv and put a pointer to whatever
	   we find there (hopefully the environment) in r2 */
	add     r2, r1, r0, lsl #2
	add     r2, r2, #4

#else
	/*
	 * uClinux stacks look a little different from normal
	 * MMU-full Linux stacks (for no good reason)
	 */
	/* pull argc, argv and envp off the stack */
	ldr r0,[sp, #0]
	ldr r1,[sp, #4]
	ldr r2,[sp, #8]
#endif

#if (defined L_crt1 ) && defined __UCLIBC_CTOR_DTOR__
#ifdef __PIC__
	/* Store the address of _init in r3 as an argument to main() */
	adr r5, .L_init
	ldr r3, .L_init
	add r3, r3, r5

	/* Push _fini onto the stack as the final argument to main() */
	ldr r4, .L_init + 4
	add r4, r4, r5
#else
	/* Store the address of _init in r3 as an argument to main() */
	ldr r3, =_init

	/* Push _fini onto the stack as the final argument to main() */
	ldr r4, =_fini
#endif
	stmfd sp!, {r4}

	/* Ok, now run uClibc's main() -- shouldn't return */
	bl	__uClibc_start_main
#else
	bl	__uClibc_main
#endif

	/* Crash if somehow `exit' returns anyways.  */
	bl abort

#if (defined L_crt1 ) && defined __UCLIBC_CTOR_DTOR__ && defined __PIC__
.L_init:
	.word _init - .L_init
	.word _fini - .L_init
#endif

/* We need this stuff to make gdb behave itself, otherwise
   gdb will chokes with SIGILL when trying to debug apps.
*/
	.section ".note.ABI-tag", "a"
	.align 4
	.long 1f - 0f
	.long 3f - 2f
	.long  1
0:	.asciz "GNU"
1:	.align 4
2:	.long 0
	.long 2,0,0
3:	.align 4

/* Define a symbol for the first piece of initialized data.  */
	.data
	.globl __data_start
__data_start:
	.long 0
	.weak data_start
	data_start = __data_start

