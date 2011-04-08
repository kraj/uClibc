/* vi: set sw=4 ts=4: */
/*
 * Copyright (C) 2000-2005 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Lesser General Public License version 2.1 or later.
 */

#ifndef _DL_SYSCALL_H
#define _DL_SYSCALL_H

#ifdef IS_IN_rtld

#include <features.h>

#include <sys/syscall.h>
#include <sys/mman.h>	/* MAP_ANONYMOUS -- differs between platforms */

#define stat __hide_stat
#include <fcntl.h>
#include <sys/stat.h>
#undef stat

/* Pull in whatever this particular arch's kernel thinks the kernel version of
 * struct stat should look like.  It turns out that each arch has a different
 * opinion on the subject, and different kernel revs use different names... */
#if defined(__sparc_v9__) && (__WORDSIZE == 64)
#define kernel_stat64 stat
#else
#define kernel_stat stat
#endif
#include <bits/kernel_stat.h>
#include <bits/kernel_types.h>

/* Pull in the arch specific syscall implementation */
#ifdef HAVE_DL_SYSCALLS_H
# include <dl-syscalls.h>
#endif

/* Here are the definitions for some syscalls that are used
   by the dynamic linker.  The idea is that we want to be able
   to call these before the errno symbol is dynamicly linked, so
   we use our own version here.  Note that we cannot assume any
   dynamic linking at all, so we cannot return any error codes.
   We just punt if there is an error. */
#define __NR__dl_exit __NR_exit
static __always_inline _syscall1(void, _dl_exit, int, status)

#define __NR__dl_close __NR_close
static __always_inline _syscall1(int, _dl_close, int, fd)

#define __NR__dl_open __NR_open
static __always_inline _syscall3(int, _dl_open, const char *, fn, int, flags,
                        __kernel_mode_t, mode)

#define __NR__dl_write __NR_write
static __always_inline _syscall3(unsigned long, _dl_write, int, fd,
                        const void *, buf, unsigned long, count)

#define __NR__dl_read __NR_read
static __always_inline _syscall3(unsigned long, _dl_read, int, fd,
                        const void *, buf, unsigned long, count)

#define __NR__dl_mprotect __NR_mprotect
static __always_inline _syscall3(int, _dl_mprotect, const void *, addr,
                        unsigned long, len, int, prot)

#define __NR__dl_stat __NR_stat
static __always_inline _syscall2(int, _dl_stat, const char *, file_name,
                        struct stat *, buf)

#define __NR__dl_fstat __NR_fstat
static __always_inline _syscall2(int, _dl_fstat, int, fd, struct stat *, buf)

#define __NR__dl_munmap __NR_munmap
static __always_inline _syscall2(int, _dl_munmap, void *, start, unsigned long, length)

#ifdef __NR_getxuid
# define __NR_getuid __NR_getxuid
#endif
#define __NR__dl_getuid __NR_getuid
static __always_inline _syscall0(uid_t, _dl_getuid)

#ifndef __NR_geteuid
# define __NR_geteuid __NR_getuid
#endif
#define __NR__dl_geteuid __NR_geteuid
static __always_inline _syscall0(uid_t, _dl_geteuid)

#ifdef __NR_getxgid
# define __NR_getgid __NR_getxgid
#endif
#define __NR__dl_getgid __NR_getgid
static __always_inline _syscall0(gid_t, _dl_getgid)

#ifndef __NR_getegid
# define __NR_getegid __NR_getgid
#endif
#define __NR__dl_getegid __NR_getegid
static __always_inline _syscall0(gid_t, _dl_getegid)

#ifdef __NR_getxpid
# define __NR_getpid __NR_getxpid
#endif
#define __NR__dl_getpid __NR_getpid
static __always_inline _syscall0(gid_t, _dl_getpid)

#define __NR__dl_readlink __NR_readlink
static __always_inline _syscall3(int, _dl_readlink, const char *, path, char *, buf,
                        size_t, bufsiz)

#ifdef __UCLIBC_HAS_SSP__
# include <sys/time.h>
# define __NR__dl_gettimeofday __NR_gettimeofday
static __always_inline _syscall2(int, _dl_gettimeofday, struct timeval *, tv,
				 __timezone_ptr_t, tz)
#endif

#include "../../libc/sysdeps/linux/common/mmap.c"

#else /* IS_IN_rtld */

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#define _dl_exit _exit
#define _dl_close close
#define _dl_open open
#define _dl_write write
#define _dl_read read
#define _dl_mprotect mprotect
#define _dl_stat stat
#define _dl_fstat fstat
#define _dl_munmap munmap
#define _dl_mmap mmap

#endif /* IS_IN_rtld */

#define _dl_mmap_check_error(X) (((void *)X) == MAP_FAILED)

#endif /* _DL_SYSCALL_H */
