/*
 * truncate64 syscall.  Copes with 64 bit and 32 bit machines
 * and on 32 bit machines this sends things into the kernel as
 * two 32-bit arguments (high and low 32 bits of length) that 
 * are ordered based on endianess.  It turns out endian.h has
 * just the macro we need to order things, __LONG_LONG_PAIR.
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

#if defined __UCLIBC_HAVE_LFS__ && defined __NR_truncate64
#if __WORDSIZE == 64
/* For a 64 bit machine, life is simple... */
_syscall2(int, truncate64, const char *, path, __off64_t, length);
#elif __WORDSIZE == 32
#define __NR___truncate64 __NR_truncate64
static inline _syscall3(int, __truncate64, const char *, path,
	uint32_t, length_first_half, 
	uint32_t, length_second_half);
/* The exported truncate64 function.  */
int truncate64 (const char * path, __off64_t length)
{
    uint32_t low = length & 0xffffffff;
    uint32_t high = length >> 32;
    return __truncate64(path, __LONG_LONG_PAIR (high, low));
}
#else
#error Your machine is not 64 bit or 32 bit, I am dazed and confused.
#endif
#endif /* __UCLIBC_HAVE_LFS__ */
