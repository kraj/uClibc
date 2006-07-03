/* pipe system call for Linux/MIPS */

/*see uClibc's sh/pipe.c and glibc-2.2.4's mips/pipe.S */

#include <features.h>
#include <asm/asm.h>
#include <asm/unistd.h>
#include <asm/regdef.h>

	.globl	pipe
	.ent	pipe, 0
pipe:
	li	v0,__NR_pipe
	syscall
	beqz	a3, 1f
#ifdef __PIC__
	la	t9, __syscall_error
	jr	t9
#else
	j	__syscall_error
#endif
1:
	sw	v0, 0(a0)
	sw	v1, 4(a0)
	li	v0, 0
	j	ra
	.end	pipe
	.size	pipe,.-pipe
libc_hidden_def(pipe)
