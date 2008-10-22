/* vi: set sw=4 ts=4: */
/*
 * posix_fadvise() for uClibc
 * http://www.opengroup.org/onlinepubs/009695399/functions/posix_fadvise.html
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

/* need to hide the 64bit prototype or the strong_alias()
 * will fail when __NR_fadvise64_64 doesnt exist */
#define posix_fadvise64 __hideposix_fadvise64

#include <sys/syscall.h>
#include <fcntl.h>

#undef posix_fadvise64

#ifdef __NR_fadvise64
#define __NR_posix_fadvise __NR_fadvise64
/* get rid of following conditional when
   all supported arches are having INTERNAL_SYSCALL defined
*/
#ifdef INTERNAL_SYSCALL
int posix_fadvise(int fd, off_t offset, off_t len, int advice)
{
	INTERNAL_SYSCALL_DECL(err);
	int ret = (int) (INTERNAL_SYSCALL(posix_fadvise, err, 5, fd,
	 __LONG_LONG_PAIR (offset >> 31, offset), len, advice));
    if (INTERNAL_SYSCALL_ERROR_P (ret, err))
      return INTERNAL_SYSCALL_ERRNO (ret, err);
    return 0;
}
#else
static __inline__ int syscall_posix_fadvise(int fd, off_t offset1, off_t offset2, off_t len, int advice);
#define __NR_syscall_posix_fadvise __NR_fadvise64
_syscall5(int, syscall_posix_fadvise, int, fd, off_t, offset1,
          off_t, offset2, off_t, len, int, advice)

int posix_fadvise(int fd, off_t offset, off_t len, int advice)
{
	int ret = syscall_posix_fadvise(fd, __LONG_LONG_PAIR (offset >> 31, offset), len, advice);
	if (ret == -1)
		return errno;
	return ret;
}

#endif

#if defined __UCLIBC_HAS_LFS__ && (!defined __NR_fadvise64_64 || !defined _syscall6)
extern __typeof(posix_fadvise) posix_fadvise64;
strong_alias(posix_fadvise,posix_fadvise64)
#endif

#else
int posix_fadvise(int fd attribute_unused, off_t offset attribute_unused, off_t len attribute_unused, int advice attribute_unused)
{
#warning This is not correct as far as SUSv3 is concerned.
	return ENOSYS;
}
#endif
