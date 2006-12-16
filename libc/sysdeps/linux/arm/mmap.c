/* vi: set sw=4 ts=4: */
/*
 * _mmap() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>

libc_hidden_proto (mmap)

#if defined __ARM_EABI__
#define __NR__mmap __NR_mmap2
#else
#define __NR__mmap __NR_mmap
#endif
static inline _syscall6 (__ptr_t, _mmap, __ptr_t, addr, size_t, len,
                         int, prot, int, flags, int, fd, __off_t, offset);

__ptr_t mmap(__ptr_t addr, size_t len, int prot,
			 int flags, int fd, __off_t offset)
{
  return (__ptr_t) _mmap (addr, len, prot, flags, fd, offset);
}

libc_hidden_def (mmap)
