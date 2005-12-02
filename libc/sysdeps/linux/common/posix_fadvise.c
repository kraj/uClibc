/* vi: set sw=4 ts=4: */
/*
 * posix_fadvise() for uClibc
 * http://www.opengroup.org/onlinepubs/009695399/functions/posix_fadvise.html
 *
 * Copyright (C) 2000-2005 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

/* need to hide the posix_fadvise64 prototype or the weak_alias()
 * will fail when __NR_fadvise64_64 doesnt exist */
#define posix_fadvise64 __hide_posix_fadvise64
#include "syscalls.h"
#include <fcntl.h>
#undef posix_fadvise64

#ifdef __NR_fadvise64
#define __NR_posix_fadvise __NR_fadvise64
_syscall4(int, posix_fadvise, int, fd, off_t, offset,
          off_t, len, int, advice);

#if defined __UCLIBC_HAS_LFS__ && !defined __NR_fadvise64_64
weak_alias(posix_fadvise, posix_fadvise64);
#endif

#else
int posix_fadvise(int fd, off_t offset, off_t len, int advice)
{
	__set_errno(ENOSYS);
	return -1;
}
#endif
