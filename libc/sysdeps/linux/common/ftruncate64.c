/*
 * ftruncate64 syscall.  Copes with 64 bit and 32 bit machines
 * and on 32 bit machines this sends things into the kernel as
 * two 32-bit arguments (high and low 32 bits of length) that 
 * are ordered based on endianess.  It turns out endian.h has
 * just the macro we need to order things (__LONG_LONG_PAIR).
 *
 *  Copyright (C) 2002  Erik Andersen <andersen@codepoet.org>
 *
 * This file is subject to the terms and conditions of the GNU
 * Lesser General Public License.  See the file COPYING.LIB in
 * the main directory of this archive for more details.
 */

#include <features.h>
#include <unistd.h>
#include <errno.h>
#include <endian.h>
#include <stdint.h>
#include <sys/syscall.h>

#if defined __UCLIBC_HAVE_LFS__ && defined __NR_ftruncate64
#if (__WORDSIZE == 64)
/* For a 64 bit machine, life is simple... */
_syscall2(int, ftruncate64, int, fd, __off64_t, length);
#elif (__WORDSIZE == 32)
#define __NR___ftruncate64 __NR_ftruncate64
static inline _syscall3(int, __ftruncate64, int, fd, int, high_length, int, low_length);
/* The exported ftruncate64 function.  */
int ftruncate64 (int fd, __off64_t length)
{
    unsigned int low = length & 0xffffffff;
    unsigned int high = length >> 32;
    return __ftruncate64(fd, __LONG_LONG_PAIR (high, low));
}
#else
#error Your machine is not 64 bit or 32 bit, I am dazed and confused.
#endif
#endif /* __UCLIBC_HAVE_LFS__ */
