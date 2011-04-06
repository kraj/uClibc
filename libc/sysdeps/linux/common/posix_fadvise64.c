/* vi: set sw=4 ts=4: */
/*
 * posix_fadvise64() for uClibc
 * http://www.opengroup.org/onlinepubs/009695399/functions/posix_fadvise.html
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <_lfs_64.h>
#include <sys/syscall.h>
#include <bits/wordsize.h>

#ifdef __NR_arm_fadvise64_64
# define __NR_fadvise64_64 __NR_arm_fadvise64_64
#endif

#if defined __NR_fadvise64_64 && defined __UCLIBC_HAS_LFS__ && __WORDSIZE == 32
# include <fcntl.h>
# include <endian.h>

int posix_fadvise64(int fd, off64_t offset, off64_t len, int advice)
{
	INTERNAL_SYSCALL_DECL (err);
# if defined __powerpc__ || defined __arm__ || defined __xtensa__
	int ret = INTERNAL_SYSCALL (fadvise64_64, err, 6, fd, advice,
				    __LONG_LONG_PAIR((long)(offset >> 32), (long)offset),
				    __LONG_LONG_PAIR((long)(len >> 32), (long)len));
# else
	int ret = INTERNAL_SYSCALL (fadvise64_64, err, 6, fd,
				    __LONG_LONG_PAIR((long)(offset >> 32), (long)offset),
				    __LONG_LONG_PAIR((long)(len >> 32), (long)len),
				    advice);
# endif
	if (INTERNAL_SYSCALL_ERROR_P (ret, err))
		return INTERNAL_SYSCALL_ERRNO (ret, err);
	return 0;
}
#endif
