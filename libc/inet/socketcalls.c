#define __FORCE_GLIBC__
#include <features.h>
#include <errno.h>
#include <syscall.h>
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


#ifdef L_accept
int accept(int s, struct sockaddr *addr, socklen_t * addrlen)
{
	unsigned long args[3];

	args[0] = s;
	args[1] = (unsigned long) addr;
	args[2] = (unsigned long) addrlen;
	return socketcall(SYS_ACCEPT, args);
}
#endif

#ifdef L_bind
int bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) myaddr;
	args[2] = addrlen;
	return socketcall(SYS_BIND, args);
}
#endif

#ifdef L_connect
int connect(int sockfd, const struct sockaddr *saddr, socklen_t addrlen)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) saddr;
	args[2] = addrlen;
	return socketcall(SYS_CONNECT, args);
}
#endif

#ifdef L_getpeername
int getpeername(int sockfd, struct sockaddr *addr, socklen_t * paddrlen)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) addr;
	args[2] = (unsigned long) paddrlen;
	return socketcall(SYS_GETPEERNAME, args);
}
#endif

#ifdef L_getsockname
int getsockname(int sockfd, struct sockaddr *addr, socklen_t * paddrlen)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) addr;
	args[2] = (unsigned long) paddrlen;
	return socketcall(SYS_GETSOCKNAME, args);
}
#endif

#ifdef L_getsockopt
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

#ifdef L_listen
int listen(int sockfd, int backlog)
{
	unsigned long args[2];

	args[0] = sockfd;
	args[1] = backlog;
	return socketcall(SYS_LISTEN, args);
}
#endif

#ifdef L_recv
/* recv, recvfrom added by bir7@leland.stanford.edu */
int recv(int sockfd, __ptr_t buffer, size_t len, int flags)
{
	unsigned long args[4];

	args[0] = sockfd;
	args[1] = (unsigned long) buffer;
	args[2] = len;
	args[3] = flags;
	return (socketcall(SYS_RECV, args));
}
#endif

#ifdef L_recvfrom
/* recv, recvfrom added by bir7@leland.stanford.edu */
int recvfrom(int sockfd, __ptr_t buffer, size_t len, int flags,
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

#ifdef L_recvmsg
int recvmsg(int sockfd, struct msghdr *msg, int flags)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) msg;
	args[2] = flags;
	return (socketcall(SYS_RECVMSG, args));
}
#endif

#ifdef L_send
/* send, sendto added by bir7@leland.stanford.edu */
int send(int sockfd, const void *buffer, size_t len, int flags)
{
	unsigned long args[4];

	args[0] = sockfd;
	args[1] = (unsigned long) buffer;
	args[2] = len;
	args[3] = flags;
	return (socketcall(SYS_SEND, args));
}
#endif

#ifdef L_sendmsg
int sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
	unsigned long args[3];

	args[0] = sockfd;
	args[1] = (unsigned long) msg;
	args[2] = flags;
	return (socketcall(SYS_SENDMSG, args));
}
#endif

#ifdef L_sendto
/* send, sendto added by bir7@leland.stanford.edu */
int sendto(int sockfd, const void *buffer, size_t len, int flags,
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

#ifdef L_setsockopt
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

#ifdef L_shutdown
/* shutdown by bir7@leland.stanford.edu */
int shutdown(int sockfd, int how)
{
	unsigned long args[2];

	args[0] = sockfd;
	args[1] = how;
	return (socketcall(SYS_SHUTDOWN, args));
}
#endif

#ifdef L_socket
int socket(int family, int type, int protocol)
{
	unsigned long args[3];

	args[0] = family;
	args[1] = type;
	args[2] = (unsigned long) protocol;
	return socketcall(SYS_SOCKET, args);
}
#endif

#ifdef L_socketpair
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

