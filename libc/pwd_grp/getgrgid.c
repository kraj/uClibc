/* vi: set sw=4 ts=4: */
/*
 * getgrgid.c - This file is part of the libc-8086/grp package for ELKS,
 * Copyright (C) 1995, 1996 Nat Friedman <ndf@linux.mit.edu>.
 * Copyright (C) 2001-2003 Erik Andersen <andersee@debian.org>
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <paths.h>
#include <errno.h>
#include "config.h"


#ifdef __UCLIBC_HAS_THREADS__
#include <pthread.h>
static pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;
# define LOCK   pthread_mutex_lock(&mylock)
# define UNLOCK pthread_mutex_unlock(&mylock);
#else
# define LOCK
# define UNLOCK
#endif


/* Search for an entry with a matching group ID.  */
int getgrgid_r (gid_t gid, struct group *group, char *buffer, 
	size_t buflen, struct group **result)
{
	int grp_fd;
	if ((grp_fd = open(_PATH_GROUP, O_RDONLY)) < 0)
		return errno;

	*result = NULL;
	while (__getgrent_r(group, buffer, buflen, grp_fd) == 0) {
		if (group->gr_gid == gid) {
			close(grp_fd);
			*result = group;
			return 0;
		}
	}

	close(grp_fd);
	return EINVAL;
}

struct group *getgrgid(const gid_t gid)
{
	int ret;
	struct group *result;
	static struct group grp;
	static char line_buff[GRP_BUFFER_SIZE];

	LOCK;
	if ((ret=getgrgid_r(gid, &grp, line_buff,  sizeof(line_buff), &result)) == 0) {
		UNLOCK;
		return &grp;
	}
	UNLOCK;
	__set_errno(ret);
	return NULL;
}

