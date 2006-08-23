/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define __FORCE_GLIBC
#include <features.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef __UCLIBC_HAS_THREADS_NATIVE__
#include <not-cancel.h>
#endif

libc_hidden_proto(memcpy)
libc_hidden_proto(open)
libc_hidden_proto(close)
libc_hidden_proto(read)
libc_hidden_proto(write)
libc_hidden_proto(getuid)
libc_hidden_proto(geteuid)
libc_hidden_proto(gethostbyname)
libc_hidden_proto(gethostname)

#define HOSTID "/etc/hostid"

#ifdef __USE_BSD
int sethostid(long int new_id)
{
	int fd;
	int ret;

	if (geteuid() || getuid()) return __set_errno(EPERM);
#ifdef __UCLIBC_HAS_THREADS_NATIVE__
	if ((fd=open_not_cancel(HOSTID,O_CREAT|O_WRONLY,0644))<0) return -1;
	ret = write_not_cancel(fd,(void *)&new_id,sizeof(new_id)) ==
		sizeof(new_id) ? 0 : -1;
	close_not_cancel_no_status (fd);
#else
	if ((fd=open(HOSTID,O_CREAT|O_WRONLY,0644))<0) return -1;
	ret = write(fd,(void *)&new_id,sizeof(new_id)) == sizeof(new_id)
		? 0 : -1;
	close (fd);
#endif
	return ret;
}
#endif

long int gethostid(void)
{
	char host[MAXHOSTNAMELEN + 1];
	int fd, id;

	/* If hostid was already set the we can return that value.
	 * It is not an error if we cannot read this file. It is not even an
	 * error if we cannot read all the bytes, we just carry on trying...
	 */
#ifdef __UCLIBC_HAS_THREADS_NATIVE__
	if ((fd =open_not_cancel_2 (HOSTID, O_RDONLY)) >= 0 &&
	     read_not_cancel (fd, (void *) &id, sizeof (id)))
	{
		close_not_cancel_no_status (fd);
		return id;
	}
	if (fd >= 0) close_not_cancel_no_status (fd);
#else
	if ((fd=open(HOSTID,O_RDONLY))>=0 && read(fd,(void *)&id,sizeof(id)))
	{
		close (fd);
		return id;
	}
	if (fd >= 0) close (fd);
#endif

	/* Try some methods of returning a unique 32 bit id. Clearly IP
	 * numbers, if on the internet, will have a unique address. If they
	 * are not on the internet then we can return 0 which means they should
	 * really set this number via a sethostid() call. If their hostname
	 * returns the loopback number (i.e. if they have put their hostname
	 * in the /etc/hosts file with 127.0.0.1) then all such hosts will
	 * have a non-unique hostid, but it doesn't matter anyway and
	 * gethostid() will return a non zero number without the need for
	 * setting one anyway.
	 *						Mitch
	 */
	if (gethostname(host,MAXHOSTNAMELEN)>=0 && *host) {
		struct hostent *hp;
		struct in_addr in;

		if ((hp = gethostbyname(host)) == (struct hostent *)NULL)

		/* This is not a error if we get here, as all it means is that
		 * this host is not on a network and/or they have not
		 * configured their network properly. So we return the unset
		 * hostid which should be 0, meaning that they should set it !!
		 */
			return 0;
		else {
			memcpy((char *) &in, (char *) hp->h_addr, hp->h_length);

			/* Just so it doesn't look exactly like the IP addr */
			return(in.s_addr<<16|in.s_addr>>16);
		}
	}
	else return 0;

}
