/* When we enter this piece of code, the program stack looks like this:
        argc            argument counter (integer)
        argv[0]         program name (pointer)
        argv[1...N]     program args (pointers)
        argv[argc-1]    end of args (integer)
        env[0...N]      environment variables (pointers)
        NULL

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
	.global __libc_init
	.global __init_stdio
	.global __stdio_close_all
	.global _void_void_null_func

	.type   _start,%function
	.type   exit,%function
	.type   main,%function
	.type   __libc_init,%function
	.type   __init_stdio,%function
	.type   __stdio_close_all,%function
	.type   _void_void_null_func,%function

.text
_start:

	/* clear the frame pointer */
	mov     fp, #0

	/* Load argc into a1 -- its final resting place */
	ldr     a1, [sp], #4

	/* Load argv into a2 -- which its final resting place */
	mov     a2, sp

	/* Set up environ,  skip to the end of argv, and put 
	 * a pointer to whatever we find there (hopefully the
	 environment) in a3 */
	ldr     a4, .L3
	add     a3, a2, a1, lsl #2
	add     a3, a3, #4
	str     a3, [a4, #0]

	/* Tell libc to initialize whatever it needs */
        bl __libc_init		    
        bl __init_stdio
	bl      main
	bl      exit

_void_void_null_func:
	nop

.weak __libc_init
__libc_init = _void_void_null_func

.weak __init_stdio
__init_stdio = _void_void_null_func

.weak __stdio_close_all
__stdio_close_all = _void_void_null_func

.align 2
.L3:
	.word environ


.data
	.align 2
	.global __environ
	
__environ:  
	.long 0

.weak environ
environ = __environ

