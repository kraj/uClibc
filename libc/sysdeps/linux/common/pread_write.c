/* vi: set sw=4 ts=4: */
/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */
/*
 * Based in part on the files
 *		./sysdeps/unix/sysv/linux/pwrite.c,
 *		./sysdeps/unix/sysv/linux/pread.c,
 *		sysdeps/posix/pread.c
 *		sysdeps/posix/pwrite.c
 * from GNU libc 2.2.5, but reworked considerably...
 */

#include <sys/syscall.h>
#include <unistd.h>
#include <endian.h>
#include <bits/wordsize.h>
#include <cancel.h>

#ifdef __NR_pread64
# define __NR_pread __NR_pread64
#endif

#ifndef MY_PREAD
# ifdef __NR_pread
#  define __NR___syscall_pread __NR_pread
static _syscall5(ssize_t, __syscall_pread, int, fd, void *, buf,
		 size_t, count, off_t, offset_hi, off_t, offset_lo)
#  define MY_PREAD(fd, buf, count, offset) __syscall_pread(fd, buf, count, OFF_HI_LO(offset))
#  define MY_PREAD64(fd, buf, count, offset) __syscall_pread(fd, buf, count, OFF64_HI_LO(offset))
# else
#  define MY_PREAD(fd, buf, count, offset) __fake_pread_pwrite(fd, buf, count, offset, 0)
#  define MY_PREAD64(fd, buf, count, offset) __fake_pread_pwrite64(fd, buf, count, offset, 0)
# endif
#endif

#ifdef __NR_pwrite64
# define __NR_pwrite __NR_pwrite64
#endif

#ifndef MY_PWRITE
# ifdef __NR_pwrite
#  define __NR___syscall_pwrite __NR_pwrite
static _syscall5(ssize_t, __syscall_pwrite, int, fd, const void *, buf,
		 size_t, count, off_t, offset_hi, off_t, offset_lo)
#  define MY_PWRITE(fd, buf, count, offset) __syscall_pwrite(fd, buf, count, OFF_HI_LO(offset))
#  define MY_PWRITE64(fd, buf, count, offset) __syscall_pwrite(fd, buf, count, OFF64_HI_LO(offset))
# else
#  define MY_PWRITE(fd, buf, count, offset) __fake_pread_pwrite(fd, buf, count, offset, 1)
#  define MY_PWRITE64(fd, buf, count, offset) __fake_pread_pwrite64(fd, buf, count, offset, 1)
# endif
#endif

#if !defined __NR_pread || !defined __NR_pwrite
static ssize_t __fake_pread_write(int fd, void *buf,
				  size_t count, off_t offset,
				  int do_pwrite)
{
	int save_errno;
	ssize_t result;
	off_t old_offset;

	/* Since we must not change the file pointer preserve the
	 * value so that we can restore it later.  */
	if ((old_offset = __NC(lseek)(fd, 0, SEEK_CUR)) == (off_t) -1)
		return -1;

	/* Set to wanted position.  */
	if (__NC(lseek)(fd, offset, SEEK_SET) == (off_t) -1)
		return -1;

	if (do_pwrite == 1) {
		/* Write the data.  */
		result = __NC(write)(fd, buf, count);
	} else {
		/* Read the data.  */
		result = __NC(read)(fd, buf, count);
	}

	/* Now we have to restore the position.  If this fails we
	 * have to return this as an error.  */
	save_errno = errno;
	if (__NC(lseek)(fd, old_offset, SEEK_SET) == (off_t) -1)
	{
		if (result == -1)
			__set_errno(save_errno);
		return -1;
	}
	__set_errno(save_errno);
	return(result);
}

# if defined __UCLIBC_HAS_LFS__ && __WORDSIZE == 32
static ssize_t __fake_pread_write64(int fd, void *buf,
				    size_t count, off64_t offset,
				    int do_pwrite)
{
	int save_errno;
	ssize_t result;
	off64_t old_offset;

	/* Since we must not change the file pointer preserve the
	 * value so that we can restore it later.  */
	if ((old_offset = __NC(lseek64)(fd, 0, SEEK_CUR)) == (off64_t) -1)
		return -1;

	/* Set to wanted position.  */
	if (__NC(lseek64)(fd, offset, SEEK_SET) == (off64_t) -1)
		return -1;

	if (do_pwrite == 1) {
		/* Write the data.  */
		result = __NC(write)(fd, buf, count);
	} else {
		/* Read the data.  */
		result = __NC(read)(fd, buf, count);
	}

	/* Now we have to restore the position. */
	save_errno = errno;
	if (__NC(lseek64)(fd, old_offset, SEEK_SET) == (off64_t) -1) {
		if (result == -1)
			__set_errno (save_errno);
		return -1;
	}
	__set_errno (save_errno);
	return result;
}
# endif
#endif /* ! __NR_pread || ! __NR_pwrite */

static ssize_t __NC(pread)(int fd, void *buf, size_t count, off_t offset)
{
	return MY_PREAD(fd, buf, count, offset);
}
CANCELLABLE_SYSCALL(ssize_t, pread, (int fd, void *buf, size_t count, off_t offset),
		    (fd, buf, count, offset))

static ssize_t __NC(pwrite)(int fd, const void *buf, size_t count, off_t offset)
{
	return MY_PWRITE(fd, buf, count, offset);
}
CANCELLABLE_SYSCALL(ssize_t, pwrite, (int fd, const void *buf, size_t count, off_t offset),
		    (fd, buf, count, offset))

#ifdef __UCLIBC_HAS_LFS__
# if __WORDSIZE == 32
static ssize_t __NC(pread64)(int fd, void *buf, size_t count, off64_t offset)
{
	return MY_PREAD64(fd, buf, count, offset);
}
CANCELLABLE_SYSCALL(ssize_t, pread64, (int fd, void *buf, size_t count, off64_t offset),
		    (fd, buf, count, offset))

static ssize_t __NC(pwrite64)(int fd, const void *buf, size_t count, off64_t offset)
{
	return MY_PWRITE64(fd, buf, count, offset);
}
CANCELLABLE_SYSCALL(ssize_t, pwrite64, (int fd, const void *buf, size_t count, off64_t offset),
		    (fd, buf, count, offset))
# else
#  ifdef __LINUXTHREADS_OLD__
weak_alias(pread,pread64)
weak_alias(pwrite,pwrite64)
#  else
strong_alias_untyped(pread,pread64)
strong_alias_untyped(pwrite,pwrite64)
#  endif
# endif
#endif
