#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H

#include <features.h>
#include <errno.h>
#include <sys/types.h>
#include <linux/socket.h>

#ifdef _MIT_POSIX_THREADS
#include <pthread/mit/posix.h>
#endif

__BEGIN_DECLS

/* struct msghdr is not defined in linux 1.2.  This will allow sendmsg
   and recvmsg in libc 5.2.9 to compile under 1.2.x and shouldn't cause
   any problem for 1.3.x */
struct msghdr;

/* Create a new socket of type TYPE in domain DOMAIN, using
   protocol PROTOCOL.  If PROTOCOL is zero, one is chosen
   automatically.  Returns a file descriptor for the new socket,
   or -1 for errors.  */
int socket __P ((int __family, int __type, int __protocol));

/* Create two new sockets, of type TYPE in domain DOMAIN and using
   protocol PROTOCOL, which are connected to each other, and put file
   descriptors for them in FDS[0] and FDS[1].  If PROTOCOL is zero,
   one will be chosen automatically.  Returns 0 on success, -1
   for errors.  */
int socketpair __P ((int __family, int __type, int __protocol,
		int __sockvec[2]));

/* Give the socket FD the local address ADDR (which is LEN bytes
   long).  */
int bind __P ((int __sockfd, __const struct sockaddr *__my_addr,
		int __addrlen));

/* Open a connection on socket FD to peer at ADDR (which LEN bytes
   long). For connectionless socket types, just set the default
   address to send to and the only address from which to accept
   transmissions.  Return 0 on success, -1 for errors.  */
int connect __P ((int __sockfd, __const struct sockaddr *__serv_addr,
		int __addrlen));

/* Prepare to accept connections on socket FD.
   N connection requests will be queued before further requests are
   refused. Returns 0 on success, -1 for errors.  */
int listen __P ((int __sockfd, int __n));

/* Await a connection on socket FD.
   When a connection arrives, open a new socket to communicate with it,
   set *ADDR (which is *ADDR_LEN bytes long) to the address of the
   connecting peer and *ADDR_LEN to the address's actual length, and
   return the new socket's descriptor, or -1 for errors.  */
int accept __P ((int __sockfd, __const struct sockaddr *__peer,
		int *__paddrlen));

/* Put the current value for socket FD's option OPTNAME at protocol
   level LEVEL into OPTVAL (which is *OPTLEN bytes long), and set
   *OPTLEN to the value's actual length. Returns 0 on success, -1 for
   errors.  */
int getsockopt __P ((int __s, int __level, int __optname,
		void *__optval, int *__optlen));

/* Set socket FD's option OPTNAME at protocol level LEVEL
   to *OPTVAL (which is OPTLEN bytes long).
   Returns 0 on success, -1 for errors.  */
int setsockopt __P ((int __s, int __level, int __optname,
		__const void *__optval, int optlen));

/* Put the local address of FD into *ADDR and its length in *LEN.  */
int getsockname __P ((int __sockfd, struct sockaddr *__addr,
		int *__paddrlen));

/* Put the address of the peer connected to socket FD into *ADDR
   (which is *LEN bytes long), and its actual length into *LEN.  */
int getpeername __P ((int __sockfd, struct sockaddr *__peer,
		int *__paddrlen));

/* Send N bytes of BUF to socket FD.  Returns the number sent or -1. */
int send __P ((int __sockfd, __const void *__buff, size_t __len,
		 unsigned int __flags));

/* Read N bytes into BUF from socket FD.
   Returns the number read or -1 for errors.  */
int recv __P ((int __sockfd, void *__buff, size_t __len,
		 unsigned int __flags));

/* Send N bytes of BUF on socket FD to peer at address ADDR (which is
   ADDR_LEN bytes long).  Returns the number sent, or -1 for errors. */
int sendto __P ((int __sockfd, __const void *__buff, size_t __len,
		 unsigned int __flags, __const struct sockaddr *__to,
		 int __tolen));

/* Read N bytes into BUF through socket FD.
   If ADDR is not NULL, fill in *ADDR_LEN bytes of it with tha address
   of the sender, and store the actual size of the address in
   *ADDR_LEN. Returns the number of bytes read or -1 for errors. */
int recvfrom __P ((int __sockfd, void *__buff, size_t __len,
		 unsigned int __flags, struct sockaddr *__from,
		 int *__fromlen));

/* Send a message described MESSAGE on socket FD.
   Returns the number of bytes sent, or -1 for errors.  */
extern int sendmsg __P ((int __fd, __const struct msghdr *__message,
			unsigned int __flags));

/* Receive a message as described by MESSAGE from socket FD.
   Returns the number of bytes read or -1 for errors.  */
extern int recvmsg __P ((int __fd, struct msghdr *__message,
			unsigned int __flags));
 
/* Shut down all or part of the connection open on socket FD.
   HOW determines what to shut down:
	0 = No more receptions;
	1 = No more transmissions;
	2 = No more receptions or transmissions.
   Returns 0 on success, -1 for errors.  */
int shutdown __P ((int __sockfd, int __how));


/* belongs here or elsewhere? */
int rcmd __P ((char **__ahost, unsigned short __inport,
		__const char *__locuser, __const char *__remuser,
		__const char *__cmd, int *__fd2p));
int rresvport __P ((int *__port));
int ruserok __P ((__const char *__rhost, int __superuser,
		__const char *__ruser, __const char *__luser));
int rexec __P ((char **__ahost, int __inport, __const char *__user,
		 __const char *__passwd, __const char *__cmd,
		 int *__fd2p));

__END_DECLS

#endif /* _SYS_SOCKET_H */
