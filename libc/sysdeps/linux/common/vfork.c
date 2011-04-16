/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>

#if (defined __NR_vfork || (defined __ARCH_USE_MMU__ && defined __NR_fork)) && (defined __USE_BSD || defined __USE_XOPEN_EXTENDED)
# include <unistd.h>
extern __typeof(vfork) __vfork attribute_hidden;

# ifdef __NR_vfork
#  define __NR___vfork __NR_vfork
_syscall0(pid_t, __vfork)
# else
/* Trivial implementation for arches that lack vfork */
pid_t __vfork(void)
{
    return fork();
}
# endif
strong_alias(__vfork,vfork)
libc_hidden_weak(vfork)
#endif
