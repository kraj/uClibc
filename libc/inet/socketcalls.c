#define __FORCE_GLIBC
#include <features.h>
#include <errno.h>
#include <syscall.h>

#ifdef __NR_socketcall
#include <sys/socket.h>
extern int socketcall(int call, unsigned long *args);

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
#include <sys/socket.h>
#ifdef __NR_accept
#define __NR_accept	__NR___libc_accept
_syscall3(int, __libc_accept, int, call, struct sockaddr *, addr, socklen_t *,addrlen);
weak_alias(__libc_accept, accept);
#else
int __libc_accept(int s, struct sockaddr *addr, socklen_t * addrlen)
{
	unsigned long args[3];

	args[0] = s;
	args[1] = (unsigned long) addr;
	args[2] = (unsigned long) addrlen;
	return socketcall(SYS_ACCEPT, args);
}
#endif
weak_alias(__libc_accept, accept);
#endif

#ifdef L_bind
#include <sys/socket.h>
#ifdef __NR_bind
_syscall3(int, bind, int, sockfd, const struct sockaddr *, myaddr, socklen_t, addrlen);
#else
int bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) myaddr;
	args[2] = addrlen;
	return socketcall(SYS_BIND, args);
}
#endif
#endif

#ifdef L_connect
#include <sys/socket.h>
#ifdef __NR_connect
#define __NR_connect __NR___libc_connect
_syscall3(int, __libc_connect, int, sockfd, const struct sockaddr *, saddr, socklen_t, addrlen);
#else
int __libc_connect(int sockfd, const struct sockaddr *saddr, socklen_t addrlen)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) saddr;
	args[2] = addrlen;
	return socketcall(SYS_CONNECT, args);
}
#endif
weak_alias(__libc_connect, connect);
#endif

#ifdef L_getpeername
#include <sys/socket.h>
#ifdef __NR_getpeername
_syscall3(int, getpeername, int, sockfd, struct, sockaddr *, addr, socklen_t *,paddrlen);
#else
int getpeername(int sockfd, struct sockaddr *addr, socklen_t * paddrlen)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) addr;
	args[2] = (unsigned long) paddrlen;
	return socketcall(SYS_GETPEERNAME, args);
}
#endif
#endif

#ifdef L_getsockname
#include <sys/socket.h>
#ifdef __NR_getsockname
_syscall3(int, getsockname, int, sockfd, struct, sockaddr *, addr, socklen_t *,paddrlen);
#else
int getsockname(int sockfd, struct sockaddr *addr, socklen_t * paddrlen)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) addr;
	args[2] = (unsigned long) paddrlen;
	return socketcall(SYS_GETSOCKNAME, args);
}
#endif
#endif

#ifdef L_getsockopt
#include <sys/socket.h>
#ifdef __NR_getsockopt
_syscall5(int, getsockopt, int, fd, int, level, int, optname, __ptr_t, optval, socklen_t *, optlen);
#else
int getsockopt(int fd, int level, int optname, __ptr_t optval,
		   socklen_t * optlen)
{
	unsigned long args[5];

	args[0] = fd;
	args[1] = level;
	args[2] = optname;
	args[3] = (unsigned long) optval;
	args[4] = (unsigned long) optlen;
	return (socketcall(SYS_GETSOCKOPT, args));
}
#endif
#endif

#ifdef L_listen
#include <sys/socket.h>
#ifdef __NR_listen
_syscall2(int, listen, int, sockfd, int, backlog);
#else
int listen(int sockfd, int backlog)
{
	unsigned long args[2];

	args[0] = sockfd;
	args[1] = backlog;
	return socketcall(SYS_LISTEN, args);
}
#endif
#endif

#ifdef L_recv
#include <sys/socket.h>
#ifdef __NR_recv
#define __NR_recv __NR___libc_recv
_syscall4(int, __libc_recv, int, sockfd, __ptr_t, buffer, size_t, len, int, flags);
#else
/* recv, recvfrom added by bir7@leland.stanford.edu */
int __libc_recv(int sockfd, __ptr_t buffer, size_t len, int flags)
{
	unsigned long args[4];

	args[0] = sockfd;
	args[1] = (unsigned long) buffer;
	args[2] = len;
	args[3] = flags;
	return (socketcall(SYS_RECV, args));
}
#endif
weak_alias(__libc_recv, recv);
#endif

#ifdef L_recvfrom
#include <sys/socket.h>
#ifdef __NR_recvfrom
#define __NR_recvfrom __NR___libc_recvfrom
_syscall6(int, __libc_recvfrom, int, sockfd, __ptr_t, buffer, size_t, len, int, flags, 
	struct sockaddr *, to, socklen_t *, tolen);
#else
/* recv, recvfrom added by bir7@leland.stanford.edu */
int __libc_recvfrom(int sockfd, __ptr_t buffer, size_t len, int flags,
		 struct sockaddr *to, socklen_t * tolen)
{
	unsigned long args[6];

	args[0] = sockfd;
	args[1] = (unsigned long) buffer;
	args[2] = len;
	args[3] = flags;
	args[4] = (unsigned long) to;
	args[5] = (unsigned long) tolen;
	return (socketcall(SYS_RECVFROM, args));
}
#endif
weak_alias(__libc_recvfrom, recvfrom);
#endif

#ifdef L_recvmsg
#include <sys/socket.h>
#ifdef __NR_recvmsg
#define __NR_recvmsg __NR___libc_recvmsg
_syscall3(int, __libc_recvmsg, int, sockfd, struct msghdr *, msg, int, flags);
#else
int __libc_recvmsg(int sockfd, struct msghdr *msg, int flags)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) msg;
	args[2] = flags;
	return (socketcall(SYS_RECVMSG, args));
}
#endif
weak_alias(__libc_recvmsg, recvmsg);
#endif

#ifdef L_send
#include <sys/socket.h>
#ifdef __NR_send
#define __NR_send __NR___libc_send
_syscall4(int, __libc_send, int, sockfd, const void *, buffer, size_t, len, int, flags);
#else
/* send, sendto added by bir7@leland.stanford.edu */
int __libc_send(int sockfd, const void *buffer, size_t len, int flags)
{
	unsigned long args[4];

	args[0] = sockfd;
	args[1] = (unsigned long) buffer;
	args[2] = len;
	args[3] = flags;
	return (socketcall(SYS_SEND, args));
}
#endif
weak_alias(__libc_send, send);
#endif

#ifdef L_sendmsg
#include <sys/socket.h>
#ifdef __NR_sendmsg
#define __NR_sendmsg __NR___libc_sendmsg
_syscall3(int, __libc_sendmsg, int, sockfd, const struct msghdr *, msg, int, flags);
#else
int __libc_sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) msg;
	args[2] = flags;
	return (socketcall(SYS_SENDMSG, args));
}
#endif
weak_alias(__libc_sendmsg, sendmsg);
#endif

#ifdef L_sendto
#include <sys/socket.h>
#ifdef __NR_sendto
#define __NR_sendto __NR___libc_sendto
_syscall6(int, __libc_sendto, int, sockfd, const void *, buffer, size_t, len, 
	int, flags, const struct sockaddr *, to, socklen_t, tolen);
#else
/* send, sendto added by bir7@leland.stanford.edu */
int __libc_sendto(int sockfd, const void *buffer, size_t len, int flags,
	   const struct sockaddr *to, socklen_t tolen)
{
	unsigned long args[6];

	args[0] = sockfd;
	args[1] = (unsigned long) buffer;
	args[2] = len;
	args[3] = flags;
	args[4] = (unsigned long) to;
	args[5] = tolen;
	return (socketcall(SYS_SENDTO, args));
}
#endif
weak_alias(__libc_sendto, sendto);
#endif

#ifdef L_setsockopt
#include <sys/socket.h>
#ifdef __NR_setsockopt
_syscall5(int, setsockopt, int, fd, int, level, int, optname, const void *, optval, socklen_t, optlen);
#else
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
	return (socketcall(SYS_SETSOCKOPT, args));
}
#endif
#endif

#ifdef L_shutdown
#include <sys/socket.h>
#ifdef __NR_shutdown
_syscall2(int, shutdown, int, sockfd, int, how);
#else
/* shutdown by bir7@leland.stanford.edu */
int shutdown(int sockfd, int how)
{
	unsigned long args[2];

	args[0] = sockfd;
	args[1] = how;
	return (socketcall(SYS_SHUTDOWN, args));
}
#endif
#endif

#ifdef L_socket
#include <sys/socket.h>
#ifdef __NR_socket
_syscall3(int, socket, int, family, int, type, int, protocol);
#else
int socket(int family, int type, int protocol)
{
	unsigned long args[3];

	args[0] = family;
	args[1] = type;
	args[2] = (unsigned long) protocol;
	return socketcall(SYS_SOCKET, args);
}
#endif
#endif

#ifdef L_socketpair
#include <sys/socket.h>
#ifdef __NR_socketpair
_syscall4(int, socketpair, int, family, int, type, int, protocol, int, sockvec[2]);
#else
int socketpair(int family, int type, int protocol, int sockvec[2])
{
	unsigned long args[4];

	args[0] = family;
	args[1] = type;
	args[2] = protocol;
	args[3] = (unsigned long) sockvec;
	return socketcall(SYS_SOCKETPAIR, args);
}
#endif
#endif

