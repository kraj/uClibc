#include <features.h>
#include <sys/syscall.h>

#ifndef __NR_vfork

/* No vfork so use fork instead */
hidden_strong_alias(__fork,__vfork)
weak_alias(vfork,__libc_fork)

#else

.text
.global __vfork
.hidden	__vfork
.type   __vfork,@function
.type   __syscall_error,@function

__vfork:
	li	0, __NR_vfork
	sc
	bnslr+
	b	__syscall_error

.size __vfork,.-__vfork
#endif
strong_alias(__vfork,vfork)
