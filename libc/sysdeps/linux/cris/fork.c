/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "sysdep.h"

#if defined __NR_fork && defined __ARCH_USE_MMU__
# include <cancel.h>

SYSCALL__ (fork, 0)
	/* R1 is now 0 for the parent and 1 for the child.  Decrement it to
	   make it -1 (all bits set) for the parent, and 0 (no bits set)
	   for the child.  Then AND it with R0, so the parent gets
	   R0&-1==R0, and the child gets R0&0==0.  */
     /* i dunno what the blurb above is useful for. we just return. */
__asm__("ret\n\tnop");

lt_strong_alias(fork)
lt_libc_hidden(fork)
#endif
