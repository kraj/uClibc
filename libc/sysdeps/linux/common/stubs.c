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
# undef __NR_sync_file_range
#endif

#ifndef __UCLIBC_HAS_SOCKET__
# undef __NR_accept
# undef __NR_accept4
# undef __NR_bind
# undef __NR_connect
# undef __NR_getpeername
# undef __NR_getsockname
# undef __NR_getsockopt
# undef __NR_listen
# undef __NR_recv
# undef __NR_recvfrom
# undef __NR_recvmsg
# undef __NR_send
# undef __NR_sendmsg
# undef __NR_sendto
# undef __NR_setsockopt
# undef __NR_shutdown
# undef __NR_socket
# undef __NR_socketcall
# undef __NR_socketpair
#endif

#ifndef __NR_accept
make_stub(accept)
#endif

#ifndef __NR_accept4
make_stub(accept4)
#endif

#ifndef __NR_bdflush
make_stub(bdflush)
#endif

#ifndef __NR_bind
make_stub(bind)
#endif

#ifndef __NR_capget
make_stub(capget)
#endif

#ifndef __NR_capset
make_stub(capset)
#endif

#ifndef __NR_connect
make_stub(connect)
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

#ifndef __NR_getpeername
make_stub(getpeername)
#endif

#if !defined(__NR_getpgrp) && (defined(__NR_getpgid) && (defined(__NR_getpid) || defined(__NR_getxpid)))
make_stub(getpgrp)
#endif

#ifndef __NR_getsockname
make_stub(getsockname)
#endif

#ifndef __NR_getsockopt
make_stub(getsockopt)
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

#ifndef __NR_listen
make_stub(listen)
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

#ifndef __NR_recv
make_stub(recv)
#endif

#ifndef __NR_recvfrom
make_stub(recvfrom)
#endif

#ifndef __NR_recvmsg
make_stub(recvmsg)
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

#ifndef __NR_send
make_stub(send)
#endif

#ifndef __NR_sendmsg
make_stub(sendmsg)
#endif

#ifndef __NR_sendto
make_stub(sendto)
#endif

#ifndef __NR_setsockopt
make_stub(setsockopt)
#endif

#ifndef __NR_setxattr
make_stub(setxattr)
#endif

#ifndef __NR_shutdown
make_stub(shutdown)
#endif

#if !defined(__NR_signalfd4) && !defined(__NR_signalfd)
make_stub(signalfd)
#endif

#ifndef __NR_socket
make_stub(socket)
#endif

#ifndef __NR_socketcall
make_stub(socketcall)
#endif

#ifndef __NR_socketpair
make_stub(socketpair)
#endif

#ifndef __NR_rt_sigtimedwait
make_stub(sigtimedwait)
make_stub(sigwaitinfo)
#endif

#ifndef __NR_splice
make_stub(splice)
#endif

#ifndef __NR_sync_file_range
make_stub(sync_file_range)
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

#ifndef __NR_vmsplice
make_stub(vmsplice)
#endif

#endif
