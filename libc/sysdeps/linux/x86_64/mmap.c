/* vi: set sw=4 ts=4: */
/*
 * mmap() for uClibc/x86_64
 *
 * Copyright (C) 2005 by Erik Andersen <andersen@codepoet.org>
 * Copyright (C) 2005 by Mike Frysinger <vapier@gentoo.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>

#define __NR___mmap __NR_mmap
attribute_hidden _syscall6(void *, __mmap, void *, start, size_t, length, int, prot,
          int, flags, int, fd, off_t, offset);
strong_alias(__mmap,mmap)
