.text
	.align 4
.globl vfork
	.type	 vfork,@function
vfork:
	popl %ecx
	movl $190,%eax
#APP
	int $0x80
#NO_APP
	cmpl $-4095,%eax
	jae .L5
	jmp *%ecx
	.p2align 4,,7
.L5:
	pushl %ecx
	negl %eax
	movl %eax,errno
	movl $-1,%eax
	ret
.Lfe1:
	.size	 vfork,.Lfe1-vfork
