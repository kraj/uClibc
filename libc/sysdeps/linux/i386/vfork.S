/*
 * June 3, 2003             Erik Andersen
 *
 */

#include <features.h>
#include <sys/syscall.h>

#ifndef __NR_vfork

/* No vfork so use fork instead */
strong_alias(fork,vfork)
strong_alias(vfork,__libc_fork)

#else

.text
.global vfork
.type   vfork,%function
.align 1<<4

vfork:
	popl %ecx
	movl $__NR_vfork,%eax
	int $0x80
	pushl %ecx
	cmpl $-4095,%eax
	jae __syscall_error
	ret

.size vfork,.-vfork
#endif
libc_hidden_def(vfork)
