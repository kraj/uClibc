/* vi: set sw=4 ts=4:
 *
 * Copyright (C) 2006 by Steven J. Hill <sjhill@realitydiluted.com>
 * Copyright (C) 2002-2005 by Erik Andersen <andersen@uclibc.org>
 *
 * New version based heavily on the files:
 *      sysdeps/linux/sysv/linux/mips/pread.c,
 *      sysdeps/linux/sysv/linux/mips/pread64.c,
 *      sysdeps/linux/sysv/linux/mips/pwrite.c,
 *      sysdeps/linux/sysv/linux/mips/pwrite64.c
 * from GNU libc 2.3.5, but with minor rework.
 *
 * Originally based in part on the files:
 *		sysdeps/unix/sysv/linux/pwrite.c,
 *		sysdeps/unix/sysv/linux/pread.c, 
 *		sysdeps/posix/pread.c,
 *		sysdeps/posix/pwrite.c
 * from GNU libc 2.2.5, but reworked considerably.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#define _LARGEFILE64_SOURCE
#include <features.h>
#undef __OPTIMIZE__
/* We absolutely do _NOT_ want interfaces silently
 *  *  * renamed under us or very bad things will happen... */
#ifdef __USE_FILE_OFFSET64
# undef __USE_FILE_OFFSET64
#endif


#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#ifdef __UCLIBC_HAS_THREADS_NATIVE__
#include <sysdep-cancel.h>
#endif

#ifdef __NR_pread64             /* Newer kernels renamed but it's the same.  */
# ifdef __NR_pread
#  error "__NR_pread and __NR_pread64 both defined???"
# endif
# define __NR_pread __NR_pread64
#endif

#ifdef __NR_pread
ssize_t
__libc_pread (int fd, void *buf, size_t count, off_t offset)
{
	ssize_t result;

#if _MIPS_SIM != _ABI64
	assert (sizeof (offset) == 4);
#endif

	if (SINGLE_THREAD_P)
	{
		/* First try the syscall.  */
#if _MIPS_SIM == _ABIN32 || _MIPS_SIM == _ABI64
		result = INLINE_SYSCALL (pread, 4, fd, buf, count, offset);
#else
		result = INLINE_SYSCALL (pread, 6, fd, buf, count, 0,
			__LONG_LONG_PAIR (offset >> 31, offset));
#endif
		return result;
	}

	int oldtype = LIBC_CANCEL_ASYNC ();

	/* First try the syscall.  */
#if _MIPS_SIM == _ABIN32 || _MIPS_SIM == _ABI64
	result = INLINE_SYSCALL (pread, 4, fd, buf,, count, offset);
#else
	result = INLINE_SYSCALL (pread, 6, fd, buf, count, 0,
		__LONG_LONG_PAIR (offset >> 31, offset));
#endif

	LIBC_CANCEL_RESET (oldtype);

	return result;
}
weak_alias (__libc_pread, pread)

#if defined __UCLIBC_HAS_LFS__ 
ssize_t
__libc_pread64 (int fd, void *buf, size_t count, off64_t offset)
{
	ssize_t result;

	if (SINGLE_THREAD_P)
	{
		/* First try the syscall.  */
#if _MIPS_SIM == _ABIN32 || _MIPS_SIM == _ABI64
	return INLINE_SYSCALL (pread, 4, fd, buf, count, offset);
#else
	return INLINE_SYSCALL (pread, 6, fd, buf, count, 0,
		__LONG_LONG_PAIR ((off_t) (offset >> 32),
		(off_t) (offset & 0xffffffff)));
#endif
	}

	int oldtype = LIBC_CANCEL_ASYNC ();

	/* First try the syscall.  */
#if _MIPS_SIM == _ABIN32 || _MIPS_SIM == _ABI64
	result = INLINE_SYSCALL (pread, 4, fd, buf, count, offset);
#else
	result = INLINE_SYSCALL (pread, 6, fd, buf, count, 0,
		__LONG_LONG_PAIR ((off_t) (offset >> 32),
		(off_t) (offset & 0xffffffff)));
#endif

	LIBC_CANCEL_RESET (oldtype);

	return result;
}
weak_alias(__libc_pread64, pread64)
#endif /* __UCLIBC_HAS_LFS__  */

#endif /* __NR_pread */

/**********************************************************************/

#ifdef __NR_pwrite64            /* Newer kernels renamed but it's the same.  */
# ifdef __NR_pwrite
#  error "__NR_pwrite and __NR_pwrite64 both defined???"
# endif
# define __NR_pwrite __NR_pwrite64
#endif

#ifdef __NR_pwrite
ssize_t
__libc_pwrite (int fd, const void *buf, size_t count, off_t offset)
{
	ssize_t result;

#if _MIPS_SIM != _ABI64
	assert (sizeof (offset) == 4);
#endif

	if (SINGLE_THREAD_P)
	{
		 /* First try the syscall.  */
#if _MIPS_SIM == _ABIN32 || _MIPS_SIM == _ABI64
		result = INLINE_SYSCALL (pwrite, 4, fd, buf, count, offset);
#else
		result = INLINE_SYSCALL (pwrite, 6, fd, buf, count, 0,
			__LONG_LONG_PAIR (offset >> 31, offset));
#endif
		return result;
	}

	int oldtype = LIBC_CANCEL_ASYNC ();

	/* First try the syscall.  */
#if _MIPS_SIM == _ABIN32 || _MIPS_SIM == _ABI64
	result = INLINE_SYSCALL (pwrite, 4, fd, buf, count, offset);
#else
	result = INLINE_SYSCALL (pwrite, 6, fd, buf, count, 0,
		__LONG_LONG_PAIR (offset >> 31, offset));
#endif

	LIBC_CANCEL_RESET (oldtype);

	return result;
}
weak_alias(__libc_pwrite, pwrite)

#if defined __UCLIBC_HAS_LFS__ 
ssize_t
__libc_pwrite64 (int fd, const void *buf, size_t count, off64_t offset)
{
	ssize_t result;

	if (SINGLE_THREAD_P)
	{
		/* First try the syscall.  */
#if _MIPS_SIM == _ABIN32 || _MIPS_SIM == _ABI64
		result = INLINE_SYSCALL (pwrite, 4, fd, buf, count, offset);
#else
		result = INLINE_SYSCALL (pwrite, 6, fd, buf, count, 0,
			__LONG_LONG_PAIR ((off_t) (offset >> 32),
			(off_t) (offset & 0xffffffff)));
#endif
		return result;
	}

	int oldtype = LIBC_CANCEL_ASYNC ();

	/* First try the syscall.  */
#if _MIPS_SIM == _ABIN32 || _MIPS_SIM == _ABI64
	result = INLINE_SYSCALL (pwrite, 4, fd, buf, count, offset);
#else
	result = INLINE_SYSCALL (pwrite, 6, fd, buf, count, 0,
		__LONG_LONG_PAIR ((off_t) (offset >> 32),
		(off_t) (offset & 0xffffffff)));
#endif

	LIBC_CANCEL_RESET (oldtype);

	return result;
}
weak_alias(__libc_pwrite64, pwrite64)
#endif /* __UCLIBC_HAS_LFS__  */

#endif /* __NR_pwrite */
