#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscall.h> directly; include <sys/syscall.h> instead."
#endif

#include <features.h>

/* Do something very evil for now.  Until we create our own syscall
 * macros, short circuit bits/syscall.h  and use asm/unistd.h instead */
#define _BITS_SYSCALL_H
#include <asm/unistd.h>

#endif /* _BITS_SYSCALLS_H */

