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

#ifdef __NR_fadvise64
# include <fcntl.h>
# include <endian.h>
# include <bits/wordsize.h>

int posix_fadvise(int fd, off_t offset, off_t len, int advice)
{
	int ret;
	INTERNAL_SYSCALL_DECL(err);
# if __WORDSIZE == 64
	ret = INTERNAL_SYSCALL(fadvise64, err, 4, fd, offset, len, advice);
# else
#  ifdef __powerpc__
	ret = INTERNAL_SYSCALL(fadvise64, err, 6, fd, /*unused*/0,
#  else
	ret = INTERNAL_SYSCALL(fadvise64, err, 5, fd,
#  endif
				   __LONG_LONG_PAIR (offset >> 31, offset), len, advice);
# endif
	if (INTERNAL_SYSCALL_ERROR_P (ret, err))
		return INTERNAL_SYSCALL_ERRNO (ret, err);
	return 0;
}
# if defined __UCLIBC_HAS_LFS__ && (!defined __NR_fadvise64_64 || __WORDSIZE == 64)
strong_alias(posix_fadvise,posix_fadvise64)
# endif
#endif
