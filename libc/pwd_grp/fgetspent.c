/* vi: set sw=4 ts=4: */
/*
 * fgetspent.c - Based on fgetpwent.c
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
#include <errno.h>
#include <stdio.h>
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

int fgetspent_r (FILE *file, struct spwd *spwd,
	char *buff, size_t buflen, struct spwd **result)
{
	int res;
	if (file == NULL) {
		return EINTR;
	}
	*result = NULL;
	res = __getspent_r(spwd, buff, buflen, fileno(file));
	*result = spwd;
	return res;
}

struct spwd *fgetspent(FILE * file)
{
	int ret;
	static char line_buff[PWD_BUFFER_SIZE];
	static struct spwd spwd;
	struct spwd *result;

	LOCK;
	if ((ret=fgetspent_r(file, &spwd, line_buff, sizeof(line_buff), &result)) == 0) {
		UNLOCK;
		return &spwd;
	}
	UNLOCK;
	__set_errno(ret);
	return NULL;
}
