/* vi: set sw=4 ts=4: */
/*
 * posix_fadvise() for uClibc
 * http://www.opengroup.org/onlinepubs/009695399/functions/posix_fadvise.html
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <fcntl.h>

#ifdef __NR_fadvise64
#define __NR_posix_fadvise __NR_fadvise64
int posix_fadvise(int fd, off_t offset, off_t len, int advice)
{
	int ret;
	INTERNAL_SYSCALL_DECL(err);
	ret = (int) (INTERNAL_SYSCALL(posix_fadvise, err, 5, fd,
	 __LONG_LONG_PAIR (offset >> 31, offset), len, advice));
    if (INTERNAL_SYSCALL_ERROR_P (ret, err))
      return INTERNAL_SYSCALL_ERRNO (ret, err);
    return 0;
}

# if defined __UCLIBC_HAS_LFS__ && !defined __NR_fadvise64_64
strong_alias(posix_fadvise,posix_fadvise64)
# endif

#endif
