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

This file now uses the register naming from the ARM Procedure Calling Standard

    Name    Number      APCS Role

    a1      0           argument 1 / integer result / scratch register / argc
    a2      1           argument 2 / scratch register / argv
    a3      2           argument 3 / scratch register / envp
    a4      3           argument 4 / scratch register
    v1      4           register variable
    v2      5           register variable
    v3      6           register variable
    v4      7           register variable
    v5      8           register variable
    sb/v6   9           static base / register variable
    sl/v7   10          stack limit / stack chunk handle / reg. variable
    fp      11          frame pointer
    ip      12          scratch register / new-sb in inter-link-unit calls
    sp      13          lower end of current stack frame
    lr      14          link address / scratch register
    pc      15          program counter
*/


.text
	.align 2
	.global __environ
	.global _start
	.global exit
	.global main

	.type   _start,%function
	.type   exit,%function
	.type   main,%function

.text
_start:

	/* clear the frame pointer */
	mov     fp, #0

	/* Load register a1 (argc) from the stack to its final resting place */
	ldr     a1, [sp], #4

	/* Copy argv pointer into a2 -- which its final resting place */
	mov     a2, sp

	/* Set up environ,  skip to the end of argv, and put 
	 * a pointer to whatever we find there (hopefully the
	 environment) in a3 */
	ldr     a4, .L3
	add     a3, a2, a1, lsl #2
	add     a3, a3, #4
	str     a3, [a4, #0]

	bl      main
	bl      exit

.align 2
.L3:
	.word __environ


.data
	.align 2
	.global __environ
	
__environ:  
	.long 0

.weak environ
environ = __environ

