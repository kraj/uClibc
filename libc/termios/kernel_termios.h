#ifndef _KERNEL_TERMIOS_H
#define _KERNEL_TERMIOS_H 1

#include <sys/types.h>

/* Pull in whatever this particular arch's kernel thinks the kernel version of
 * struct termios should look like.  It turns out that each arch has a different
 * opinion on the subject, and different kernel revs use different names... */
#define termio	    __kernel_termios
#define winsize	    __kernel_winsize
#define cc_t	    __kernel_cc_t
#define speed_t	    __kernel_speed_t
#define tcflag_t    __kernel_tcflag_t
#undef NCCS
#include <asm/termios.h> 
#define __KERNEL_NCCS	NCCS
#undef NCCS
#undef termio
#undef winsize
#undef cc_t
#undef speed_t
#undef tcflag_t

/* Now pull in libc's version of termios */
#define termios libc_termios
#include <termios.h>
#undef termios

#endif	    /* _KERNEL_TERMIOS_H */

