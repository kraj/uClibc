/* vi: set sw=4 ts=4: */
/*
 * getgrnam.c - This file is part of the libc-8086/grp package for ELKS,
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

#include <features.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <paths.h>
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

int getgrnam_r (const char *name, struct group *group,
	char *buff, size_t buflen, struct group **result)
{
	int ret;
	int group_fd;

	*result = NULL;

	if (name == NULL) {
		return EINVAL;
	}

	if ((group_fd = open(_PATH_GROUP, O_RDONLY)) < 0) {
		return ENOENT;
	}

	while ((ret=__getgrent_r(group, buff, buflen, group_fd)) == 0) {
		if (!strcmp(group->gr_name, name)) {
			close(group_fd);
			*result = group;
			return 0;
		}
	}

	close(group_fd);
	return ret;
}

struct group *getgrnam(const char *name)
{
	int ret;
	static char line_buff[PWD_BUFFER_SIZE];
	static struct group grp;
	struct group *result;

	LOCK;
	if ((ret=getgrnam_r(name, &grp, line_buff, sizeof(line_buff), &result)) == 0) {
		UNLOCK;
		return &grp;
	}
	__set_errno(ret);
	UNLOCK;
	return NULL;
}


