/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define __FORCE_GLIBC
#include <features.h>
#include <errno.h>
#include <syscall.h>
#include <sys/socket.h>

#ifdef __NR_socketcall
extern int __socketcall(int call, unsigned long *args) attribute_hidden;

/* Various socketcall numbers */
#define SYS_SOCKET      1
#define SYS_BIND        2
#define SYS_CONNECT     3
#define SYS_LISTEN      4
#define SYS_ACCEPT      5
#define SYS_GETSOCKNAME 6
#define SYS_GETPEERNAME 7
#define SYS_SOCKETPAIR  8
#define SYS_SEND        9
#define SYS_RECV        10
#define SYS_SENDTO      11
#define SYS_RECVFROM    12
#define SYS_SHUTDOWN    13
#define SYS_SETSOCKOPT  14
#define SYS_GETSOCKOPT  15
#define SYS_SENDMSG     16
#define SYS_RECVMSG     17
#endif


#ifdef L_accept
# ifdef __NR_accept
_syscall3(int, accept, int, call, struct sockaddr *, addr, socklen_t *,addrlen)
# elif defined(__NR_socketcall)
int accept(int s, struct sockaddr *addr, socklen_t * addrlen)
{
	unsigned long args[3];

	args[0] = s;
	args[1] = (unsigned long) addr;
	args[2] = (unsigned long) addrlen;
	return __socketcall(SYS_ACCEPT, args);
}
# endif
# ifndef __LINUXTHREADS_OLD__
libc_hidden_def(accept)
# else
libc_hidden_weak(accept)
strong_alias(accept,__libc_accept)
# endif
#endif

#ifdef L_bind
#ifdef __NR_bind
_syscall3(int, bind, int, sockfd, const struct sockaddr *, myaddr, socklen_t, addrlen)
#elif defined(__NR_socketcall)
int bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) myaddr;
	args[2] = addrlen;
	return __socketcall(SYS_BIND, args);
}
#endif
libc_hidden_def(bind)
#endif

#ifdef L_connect
# ifdef __NR_connect
_syscall3(int, connect, int, sockfd, const struct sockaddr *, saddr, socklen_t, addrlen)
# elif defined(__NR_socketcall)
int connect(int sockfd, const struct sockaddr *saddr, socklen_t addrlen)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) saddr;
	args[2] = addrlen;
	return __socketcall(SYS_CONNECT, args);
}
# endif
# ifndef __LINUXTHREADS_OLD__
libc_hidden_def(connect)
# else
libc_hidden_weak(connect)
strong_alias(connect,__libc_connect)
# endif
#endif

#ifdef L_getpeername
#ifdef __NR_getpeername
_syscall3(int, getpeername, int, sockfd, struct sockaddr *, addr, socklen_t *,paddrlen)
#elif defined(__NR_socketcall)
int getpeername(int sockfd, struct sockaddr *addr, socklen_t * paddrlen)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) addr;
	args[2] = (unsigned long) paddrlen;
	return __socketcall(SYS_GETPEERNAME, args);
}
#endif
#endif

#ifdef L_getsockname
#ifdef __NR_getsockname
_syscall3(int, getsockname, int, sockfd, struct sockaddr *, addr, socklen_t *,paddrlen)
#elif defined(__NR_socketcall)
int getsockname(int sockfd, struct sockaddr *addr, socklen_t * paddrlen)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) addr;
	args[2] = (unsigned long) paddrlen;
	return __socketcall(SYS_GETSOCKNAME, args);
}
#endif
libc_hidden_def(getsockname)
#endif

#ifdef L_getsockopt
#ifdef __NR_getsockopt
_syscall5(int, getsockopt, int, fd, int, level, int, optname, __ptr_t, optval, socklen_t *, optlen)
#elif defined(__NR_socketcall)
int getsockopt(int fd, int level, int optname, __ptr_t optval,
		   socklen_t * optlen)
{
	unsigned long args[5];

	args[0] = fd;
	args[1] = level;
	args[2] = optname;
	args[3] = (unsigned long) optval;
	args[4] = (unsigned long) optlen;
	return (__socketcall(SYS_GETSOCKOPT, args));
}
#endif
#endif

#ifdef L_listen
#ifdef __NR_listen
_syscall2(int, listen, int, sockfd, int, backlog)
#elif defined(__NR_socketcall)
int listen(int sockfd, int backlog)
{
	unsigned long args[2];

	args[0] = sockfd;
	args[1] = backlog;
	return __socketcall(SYS_LISTEN, args);
}
#endif
libc_hidden_def(listen)
#endif

#ifdef L_recv
# ifdef __NR_recv
_syscall4(ssize_t, recv, int, sockfd, __ptr_t, buffer, size_t, len,
	int, flags)
# elif defined(__NR_socketcall)
/* recv, recvfrom added by bir7@leland.stanford.edu */
ssize_t recv(int sockfd, __ptr_t buffer, size_t len, int flags)
{
	unsigned long args[4];

	args[0] = sockfd;
	args[1] = (unsigned long) buffer;
	args[2] = len;
	args[3] = flags;
	return (__socketcall(SYS_RECV, args));
}
# elif defined(__NR_recvfrom)
ssize_t recv(int sockfd, __ptr_t buffer, size_t len, int flags)
{
	return (recvfrom(sockfd, buffer, len, flags, NULL, NULL));
}
# endif
# ifndef __LINUXTHREADS_OLD__
libc_hidden_def(recv)
# else
libc_hidden_weak(recv)
strong_alias(recv,__libc_recv)
# endif
#endif

#ifdef L_recvfrom
# ifdef __NR_recvfrom
_syscall6(ssize_t, recvfrom, int, sockfd, __ptr_t, buffer, size_t, len,
	int, flags, struct sockaddr *, to, socklen_t *, tolen)
# elif defined(__NR_socketcall)
/* recv, recvfrom added by bir7@leland.stanford.edu */
ssize_t recvfrom(int sockfd, __ptr_t buffer, size_t len, int flags,
		 struct sockaddr *to, socklen_t * tolen)
{
	unsigned long args[6];

	args[0] = sockfd;
	args[1] = (unsigned long) buffer;
	args[2] = len;
	args[3] = flags;
	args[4] = (unsigned long) to;
	args[5] = (unsigned long) tolen;
	return (__socketcall(SYS_RECVFROM, args));
}
# endif
# ifndef __LINUXTHREADS_OLD__
libc_hidden_def(recvfrom)
# else
libc_hidden_weak(recvfrom)
strong_alias(recvfrom,__libc_recvfrom)
# endif
#endif

#ifdef L_recvmsg
# ifdef __NR_recvmsg
_syscall3(ssize_t, recvmsg, int, sockfd, struct msghdr *, msg, int, flags)
# elif defined(__NR_socketcall)
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) msg;
	args[2] = flags;
	return (__socketcall(SYS_RECVMSG, args));
}
# endif
# ifndef __LINUXTHREADS_OLD__
libc_hidden_def(recvmsg)
# else
libc_hidden_weak(recvmsg)
strong_alias(recvmsg,__libc_recvmsg)
# endif
#endif

#ifdef L_send
# ifdef __NR_send
_syscall4(ssize_t, send, int, sockfd, const void *, buffer, size_t, len, int, flags)
# elif defined(__NR_socketcall)
/* send, sendto added by bir7@leland.stanford.edu */
ssize_t send(int sockfd, const void *buffer, size_t len, int flags)
{
	unsigned long args[4];

	args[0] = sockfd;
	args[1] = (unsigned long) buffer;
	args[2] = len;
	args[3] = flags;
	return (__socketcall(SYS_SEND, args));
}
# elif defined(__NR_sendto)
ssize_t send(int sockfd, const void *buffer, size_t len, int flags)
{
	return (sendto(sockfd, buffer, len, flags, NULL, 0));
}
# endif
# ifndef __LINUXTHREADS_OLD__
libc_hidden_def(send)
# else
libc_hidden_weak(send)
strong_alias(send,__libc_send)
# endif
#endif

#ifdef L_sendmsg
# ifdef __NR_sendmsg
_syscall3(ssize_t, sendmsg, int, sockfd, const struct msghdr *, msg, int, flags)
# elif defined(__NR_socketcall)
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) msg;
	args[2] = flags;
	return (__socketcall(SYS_SENDMSG, args));
}
# endif
# ifndef __LINUXTHREADS_OLD__
libc_hidden_def(sendmsg)
# else
libc_hidden_weak(sendmsg)
strong_alias(sendmsg,__libc_sendmsg)
# endif
#endif

#ifdef L_sendto
# ifdef __NR_sendto
_syscall6(ssize_t, sendto, int, sockfd, const void *, buffer,
	size_t, len, int, flags, const struct sockaddr *, to, socklen_t, tolen)
# elif defined(__NR_socketcall)
/* send, sendto added by bir7@leland.stanford.edu */
ssize_t sendto(int sockfd, const void *buffer, size_t len, int flags,
	   const struct sockaddr *to, socklen_t tolen)
{
	unsigned long args[6];

	args[0] = sockfd;
	args[1] = (unsigned long) buffer;
	args[2] = len;
	args[3] = flags;
	args[4] = (unsigned long) to;
	args[5] = tolen;
	return (__socketcall(SYS_SENDTO, args));
}
# endif
# ifndef __LINUXTHREADS_OLD__
libc_hidden_def(sendto)
# else
libc_hidden_weak(sendto)
strong_alias(sendto,__libc_sendto)
# endif
#endif

#ifdef L_setsockopt
#ifdef __NR_setsockopt
_syscall5(int, setsockopt, int, fd, int, level, int, optname, const void *, optval, socklen_t, optlen)
#elif defined(__NR_socketcall)
/* [sg]etsockoptions by bir7@leland.stanford.edu */
int setsockopt(int fd, int level, int optname, const void *optval,
		   socklen_t optlen)
{
	unsigned long args[5];

	args[0] = fd;
	args[1] = level;
	args[2] = optname;
	args[3] = (unsigned long) optval;
	args[4] = optlen;
	return (__socketcall(SYS_SETSOCKOPT, args));
}
#endif
libc_hidden_def(setsockopt)
#endif

#ifdef L_shutdown
#ifdef __NR_shutdown
_syscall2(int, shutdown, int, sockfd, int, how)
#elif defined(__NR_socketcall)
/* shutdown by bir7@leland.stanford.edu */
int shutdown(int sockfd, int how)
{
	unsigned long args[2];

	args[0] = sockfd;
	args[1] = how;
	return (__socketcall(SYS_SHUTDOWN, args));
}
#endif
#endif

#ifdef L_socket
#ifdef __NR_socket
_syscall3(int, socket, int, family, int, type, int, protocol)
#elif defined(__NR_socketcall)
int socket(int family, int type, int protocol)
{
	unsigned long args[3];

	args[0] = family;
	args[1] = type;
	args[2] = (unsigned long) protocol;
	return __socketcall(SYS_SOCKET, args);
}
#endif
libc_hidden_def(socket)
#endif

#ifdef L_socketpair
#ifdef __NR_socketpair
_syscall4(int, socketpair, int, family, int, type, int, protocol, int *, sockvec)
#elif defined(__NR_socketcall)
int socketpair(int family, int type, int protocol, int sockvec[2])
{
	unsigned long args[4];

	args[0] = family;
	args[1] = type;
	args[2] = protocol;
	args[3] = (unsigned long) sockvec;
	return __socketcall(SYS_SOCKETPAIR, args);
}
#endif
#endif
