/*
 * ftruncate64 syscall for linux/i386
 *
 *  Copyright (C) 2002  Erik Andersen <andersen@codepoet.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License.  See the file COPYING.LIB in the main
 * directory of this archive for more details.
 */

#include <features.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>

#if defined __UCLIBC_HAVE_LFS__ && defined __NR_ftruncate64

/* Unlike some architectures, i386 can pass an off64_t directly 
 * into a syscall so we don't need to do anything special */
extern _syscall2 (int, ftruncate64, int, fd, __off64_t, length);

#endif /* __UCLIBC_HAVE_LFS__ */
