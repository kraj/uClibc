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



@ r0 = argc
@ r1 = argv
@ r2 = envp
@ sl = data segment

.text
_start:
	@ adjust the data segment base pointer
	ldr r3,=__data_start
	sub sl,sl,r3
	mov r9,sl

	ldr r3, .L3
	str r2,[r9,r3]
	
	/* Tell libc to initialize whatever it needs */
        bl __libc_init		    
        bl __init_stdio

	/* pull argc, argv and envp off the stack */
	ldr r0,[sp, #0]
	ldr r1,[sp, #4]
	ldr r2,[sp, #8]

	bl      main
/*	ldr r0,=0  */
	bl      exit

_void_void_null_func:
	mov     pc, lr

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


