/*
 * system call not available stub
 *
 * Copyright (C) 2009 Analog Devices Inc.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <errno.h>
#include <sys/syscall.h>

#ifdef __UCLIBC_HAS_STUBS__

static int enosys_stub(void)
{
	__set_errno(ENOSYS);
	return -1;
}

#define make_stub(stub) \
	link_warning(stub, #stub ": this function is not implemented") \
	strong_alias(enosys_stub, stub)

#ifndef __ARCH_USE_MMU__
# undef __NR_fork
#endif

#ifndef __UCLIBC_HAS_LFS__
# undef __NR_fadvise64
# undef __NR_fadvise64_64
# undef __NR_readahead
# undef __NR_sync_file_range
# undef __NR_splice
#endif

#ifndef __NR_bdflush
make_stub(bdflush)
#endif

#ifndef __NR_capget
make_stub(capget)
#endif

#ifndef __NR_capset
make_stub(capset)
#endif

#ifndef __NR_create_module
make_stub(create_module)
#endif

#ifndef __NR_delete_module
make_stub(delete_module)
#endif

#ifndef __NR_epoll_create
make_stub(epoll_create)
#endif

#ifndef __NR_epoll_ctl
make_stub(epoll_ctl)
#endif

#ifndef __NR_epoll_wait
make_stub(epoll_wait)
#endif

#ifndef __NR_epoll_pwait
make_stub(epoll_pwait)
#endif

#ifndef __NR_fdatasync
make_stub(fdatasync)
#endif

#ifndef __NR_flistxattr
make_stub(flistxattr)
#endif

#ifndef __NR_fork
make_stub(fork)
#endif

#ifndef __NR_fgetxattr
make_stub(fgetxattr)
#endif

#ifndef __NR_fremovexattr
make_stub(fremovexattr)
#endif

#ifndef __NR_fsetxattr
make_stub(fsetxattr)
#endif

#ifndef __NR_get_kernel_syms
make_stub(get_kernel_syms)
#endif

#if !defined(__NR_getpgrp) && (defined(__NR_getpgid) && (defined(__NR_getpid) || defined(__NR_getxpid)))
make_stub(getpgrp)
#endif

#ifndef __NR_getxattr
make_stub(getxattr)
#endif

#ifndef __NR_init_module
make_stub(init_module)
#endif

#ifndef __NR_lgetxattr
make_stub(lgetxattr)
#endif

#ifndef __NR_listxattr
make_stub(listxattr)
#endif

#ifndef __NR_llistxattr
make_stub(llistxattr)
#endif

#ifndef __NR_lremovexattr
make_stub(lremovexattr)
#endif

#ifndef __NR_lsetxattr
make_stub(lsetxattr)
#endif

#ifndef __NR_pivot_root
make_stub(pivot_root)
#endif

#ifndef __NR_query_module
make_stub(query_module)
#endif

#ifndef __NR_removexattr
make_stub(removexattr)
#endif

#ifndef __NR_sched_getaffinity
make_stub(sched_getaffinity)
#endif

#ifndef __NR_sched_setaffinity
make_stub(sched_setaffinity)
#endif

#ifndef __NR_setxattr
make_stub(setxattr)
#endif

#if !defined(__NR_signalfd4) && !defined(__NR_signalfd)
make_stub(signalfd)
#endif

#ifndef __NR_rt_sigtimedwait
make_stub(sigtimedwait)
make_stub(sigwaitinfo)
#endif

#ifndef __NR_readahead
make_stub(readahead)
#endif

#ifndef __NR_sync_file_range
make_stub(sync_file_range)
#endif

#ifndef __NR_splice
make_stub(splice)
#endif

#ifndef __NR_vmsplice
make_stub(vmsplice)
#endif

#ifndef __NR_tee
make_stub(tee)
#endif

#if !defined(__NR_umount) && !defined(__NR_umount2)
make_stub(umount)
#endif

#ifndef __NR_umount2
make_stub(umount2)
#endif

#ifndef __NR_utimensat
make_stub(futimens)
make_stub(utimensat)
# ifndef __NR_lutimes
make_stub(lutimes)
# endif
#endif

#endif
