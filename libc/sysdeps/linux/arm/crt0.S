/* When we enter this piece of code, the program stack looks like this:
        argc            argument counter (integer)
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


.text
	.global _start
	.global __uClibc_main

	.type   _start,%function
	.type   __uClibc_main,%function

.text
_start:

	/* clear the frame pointer */
	mov     fp, #0

	/* Load register r0 (argc) from the stack to its final resting place */
	ldr     r0, [sp], #4

	/* Copy argv pointer into r1 -- which its final resting place */
	mov     r1, sp

	/* Skip to the end of argv and put a pointer to whatever 
	   we find there (hopefully the environment) in r2 */
	add     r2, r1, r0, lsl #2
	add     r2, r2, #4

	/* Ok, now run uClibc's main() -- shouldn't return */
	bl	__uClibc_main

#if 0 /* this is already provided by crtbegin/crtend in the arm-elf compiler */
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
