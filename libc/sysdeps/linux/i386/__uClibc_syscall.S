.globl __uClibc_syscall

.text
	.align 4
__uClibc_syscall:
	pushl %ebp
	movl %esp,%ebp
	subl $8,%esp
	pushl %edi
	pushl %esi
	pushl %ebx
	/* movl $21,%eax */
	and $0xff,%eax
	movl 8(%ebp),%ebx
	movl 12(%ebp),%ecx
	movl 16(%ebp),%edx
	movl 20(%ebp),%esi
	movl 24(%ebp),%edi
#APP
	int $0x80
#NO_APP
	movl %eax,-4(%ebp)
	.p2align 4,,7
	cmpl $-126,-4(%ebp)
	jbe .L5
	movl -4(%ebp),%eax
	negl %eax
	movl %eax,errno
	movl $-1,-4(%ebp)
.L5:
	movl -4(%ebp),%edx
	movl %edx,-8(%ebp)
	movl -8(%ebp),%eax
	leal -20(%ebp),%esp
	popl %ebx
	popl %esi
	popl %edi
	leave
	ret
