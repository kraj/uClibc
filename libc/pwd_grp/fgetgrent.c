/* vi: set sw=4 ts=4: */
/*
 * fgetgrent.c - This file is part of the libc-8086/grp package for ELKS,
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

#include <stdio.h>
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


int fgetgrent_r (FILE *__restrict file, struct group *__restrict grp,
			char *__restrict buff, size_t buflen,
			struct group **__restrict result)
{
	int ret;
	if (file == NULL) {
		return EINTR;
	}
	*result = NULL;
	flockfile(file);
	ret = __getgrent_r(grp, buff, buflen, fileno(file));
	funlockfile(file);
	if (ret == 0) {
		*result = grp;
		return 0;
	}
	__set_errno(ret);
	return ret;
}

struct group *fgetgrent(FILE * file)
{
	int ret;
	struct group *result;
	static struct group grp;
	static char line_buff[PWD_BUFFER_SIZE];

	LOCK;
	ret=fgetgrent_r(file, &grp, line_buff, sizeof(line_buff), &result);
	if (ret == 0) {
		UNLOCK;
		return result;
	}
	UNLOCK;
	__set_errno(ret);
	return NULL;
}
