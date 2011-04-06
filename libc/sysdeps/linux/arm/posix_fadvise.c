/* vi: set sw=4 ts=4: */
/*
 * posix_fadvise() for ARM uClibc
 * http://www.opengroup.org/onlinepubs/009695399/functions/posix_fadvise.html
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */
#include <sys/syscall.h>

#ifdef __NR_arm_fadvise64_64
# include <fcntl.h>
# include <endian.h>

# define HIGH_BITS(x) (sizeof(x) > 4 ? (x) >> 32 : 0)

/* This is for the ARM version of fadvise64_64 which swaps the params
 *  * about to avoid having ABI compat issues
 *   */
int posix_fadvise(int fd, off_t offset, off_t len, int advice)
{
	INTERNAL_SYSCALL_DECL (err);
	int ret = INTERNAL_SYSCALL (arm_fadvise64_64, err, 6, fd, advice,
				    __LONG_LONG_PAIR (HIGH_BITS(offset), (long)offset),
				    __LONG_LONG_PAIR (HIGH_BITS(len), (long)len));
	if (INTERNAL_SYSCALL_ERROR_P (ret, err))
		return INTERNAL_SYSCALL_ERRNO (ret, err);
	return 0;
}
#endif
