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



@ r0 = argc
@ r1 = argv
@ r2 = envp
@ sl = data segment
#define BASEREG	    r9

.text
_start:
	@ adjust the data segment base pointer
	ldr r3,=__data_start
	sub sl,sl,r3
	mov BASEREG,sl

	ldr r3, .L3
	str r2,[BASEREG,r3]
	
	/* pull argc, argv and envp off the stack */
	ldr r0,[sp, #0]
	ldr r1,[sp, #4]
	ldr r2,[sp, #8]

	bl      main
/*	ldr r0,=0  */
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


