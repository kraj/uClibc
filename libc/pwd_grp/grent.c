/*
 * grent.c - This file is part of the libc-8086/grp package for ELKS,
 * Copyright (C) 1995, 1996 Nat Friedman <ndf@linux.mit.edu>.
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

/*
 * setgrent(), endgrent(), and getgrent() are mutually-dependent functions,
 * so they are all included in the same object file, and thus all linked
 * in together.
 */

#include <features.h>
#include <unistd.h>
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

static int grp_fd = -1;

void setgrent(void)
{
    LOCK;
    if (grp_fd != -1)
	close(grp_fd);
    grp_fd = open(_PATH_GROUP, O_RDONLY);
    UNLOCK;
}

void endgrent(void)
{
    LOCK;
    if (grp_fd != -1)
	close(grp_fd);
    grp_fd = -1;
    UNLOCK;
}

struct group *getgrent(void)
{
    LOCK;
    if (grp_fd == -1)
	return NULL;
    return __getgrent(grp_fd);
    UNLOCK;
}
