#include <features.h>
#include <sys/syscall.h>

#ifndef __NR_vfork

/* No vfork so use fork instead */
strong_alias(fork,vfork)
strong_alias(vfork,__libc_fork)

#else

.text
.global vfork
.type   vfork,@function
.type   __syscall_error,@function

vfork:
	li	0, __NR_vfork
	sc
	bnslr+
	b	__syscall_error

.size vfork,.-vfork
#endif
libc_hidden_def(vfork)
