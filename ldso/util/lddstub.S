	.text
.globl main
.globl exit
main:
exit:
#if defined(__i386__)
	movl $1,%eax
	movl $0,%ebx
	int  $0x80
#elif defined(__mc68000__)
        movel #1,%d0
        clrl %d1
        trap #0
#elif defined(__sparc__)
	mov  1,%g1
	mov  0,%o0
	t    0x10
#else
#error Only know how to support i386, m68k and sparc architectures
#endif

.globl atexit
.globl __libc_init
.globl __setfpucw
atexit:
__libc_init:
__setfpucw:
#if defined(__i386__)
	ret
#elif defined(__mc68000__)
        rts
#elif defined(__sparc__)
	ret
	nop
#else
#error Only know how to support i386, m68k and sparc architectures
#endif

	.data
.globl __fpu_control
__fpu_control:
	.long	0
