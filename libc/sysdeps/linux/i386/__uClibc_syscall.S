.text
	.align 4
.globl __uClibc_syscall
	.type	 __uClibc_syscall,@function
__uClibc_syscall:
	pushl %edi
	pushl %esi
	pushl %ebx
	and $0xff,%eax
	movl 16(%esp),%ebx
	movl 20(%esp),%ecx
	movl 24(%esp),%edx
	movl 28(%esp),%esi
	movl 32(%esp),%edi
#APP
	int $0x80
#NO_APP
	cmpl $-4095,%eax
	/* jae .L5 */
	/* cmpl $-126,%eax */
	jbe .L5
	negl %eax
	movl %eax,errno
	movl $-1,%eax
	.p2align 4,,7
.L5:
	popl %ebx
	popl %esi
	popl %edi
	ret
.Lfe1:
	.size	 __uClibc_syscall,.Lfe1-__uClibc_syscall
