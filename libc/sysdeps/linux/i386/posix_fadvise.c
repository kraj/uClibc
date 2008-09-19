/* vi: set sw=4 ts=4: */
/*
 * posix_fadvise() for uClibc
 *
 * Copyright (C) 2008 Bernhard Reutner-Fischer <uclibc@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#if defined __USE_GNU
#include <fcntl.h>


#if defined __NR_fadvise64_64 || defined __NR_fadvise64
libc_hidden_proto(posix_fadvise64)
libc_hidden_proto(posix_fadvise)
int posix_fadvise(int fd, off_t offset, off_t len, int advice)
{
	if (posix_fadvise64(fd, offset, len, advice) != 0)
		return errno;
	return 0;
}
libc_hidden_def(posix_fadvise)
#elif defined __UCLIBC_HAS_STUBS__
libc_hidden_proto(posix_fadvise)
int posix_fadvise(int fd attribute_unused, off_t offset attribute_unused,
				  off_t len attribute_unused, int advice attribute_unused)
{
	return ENOSYS;
}
libc_hidden_def(posix_fadvise)
#endif
#endif
