/*
 * nothing needed here until we want pthread support or similar
 */

#include <features.h>
/* Do something very evil for now.  Until we include our out syscall
 * macros, short circuit bits/syscall.h  and use asm/unistd.h instead */
#define _BITS_SYSCALL_H
#include <asm/unistd.h>

