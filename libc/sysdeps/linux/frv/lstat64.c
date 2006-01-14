/*
 * Syscalls for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */
/* Extracted from ../common/syscalls.c by Erik Andersen <andersen@codepoet.org>
 * Adapted to FR-V by Alexandre Oliva <aoliva@redhat.com>
 */

#define _GNU_SOURCE
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
#include <endian.h>

#include <unistd.h>
#define _SYS_STAT_H
#include <bits/stat.h>

#if defined __UCLIBC_HAS_LFS__
#define __NR___syscall_lstat64 __NR_lstat64
static inline _syscall2(int, __syscall_lstat64, const char *, file_name, struct stat64 *, buf);
strong_alias(__syscall_lstat64,lstat64)
libc_hidden_proto(lstat64)
libc_hidden_def(lstat64)
#endif
