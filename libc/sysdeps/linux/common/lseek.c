/* vi: set sw=4 ts=4: */
/*
 * lseek() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <unistd.h>

#ifdef __NR_lseek
_syscall3(__off_t, lseek, int, fildes, __off_t, offset, int, whence)
#elif defined __UCLIBC_HAS_LFS__ && defined __NR__llseek /* avoid circular dependency */
__off_t lseek(int fildes, __off_t offset, int whence)
{
	return lseek64(fildes, offset, whence);
}
#else
# include <errno.h>
__off_t lseek(int fildes, __off_t offset attribute_unused, int whence)
{
	if (fildes < 0) {
		__set_errno(EBADF);
		return -1;
	}

	switch(whence) {
		case SEEK_SET:
		case SEEK_CUR:
		case SEEK_END:
			break;
		default:
			__set_errno(EINVAL);
			return -1;
	}

	__set_errno(ENOSYS);
	return -1;
}
#endif
#ifndef __LINUXTHREADS_OLD__
libc_hidden_def(lseek)
#else
libc_hidden_weak(lseek)
strong_alias(lseek,__libc_lseek)
#endif
