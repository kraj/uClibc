/* vi: set sw=4 ts=4: */
/*
 * getpwuid.c - This file is part of the libc-8086/pwd package for ELKS,
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
#include <stdlib.h>
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

int getpwuid_r (uid_t uid, struct passwd *password,
	char *buff, size_t buflen, struct passwd **result)
{
	int passwd_fd;
	if ((passwd_fd = open(_PATH_PASSWD, O_RDONLY)) < 0)
		return errno;

	*result = NULL;
	while (__getpwent_r(password, buff, buflen, passwd_fd) == 0) {
		if (password->pw_uid == uid) {
			close(passwd_fd);
			*result = password;
			return 0;
		}
	}

	close(passwd_fd);
	return EINVAL;
}

struct passwd *getpwuid(uid_t uid)
{
	int ret;
	struct passwd *result;
	static struct passwd pwd;
	static char line_buff[PWD_BUFFER_SIZE];

	LOCK;
	if ((ret=getpwuid_r(uid, &pwd, line_buff,  sizeof(line_buff), &result)) == 0) {
		UNLOCK;
		return &pwd;
	}
	UNLOCK;
	__set_errno(ret);
	return NULL;
}

