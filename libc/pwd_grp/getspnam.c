/*
 * getspnam.c - Based on getpwnam.c
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

int getspnam_r (const char *name, struct spwd *spwd,
	char *buff, size_t buflen, struct spwd **result)
{
    int spwd_fd;

    if (name == NULL) {
	return EINVAL;
    }

    if ((spwd_fd = open(_PATH_SHADOW, O_RDONLY)) < 0)
	return errno;

    *result = NULL;
    while (__getspent_r(spwd, buff, buflen, spwd_fd) == 0)
	if (!strcmp(spwd->sp_namp, name)) {
	    close(spwd_fd);
	    *result = spwd;
	    return 0;
	}

    close(spwd_fd);
    return EINVAL;
}

struct spwd *getspnam(const char *name)
{
    int ret;
    static char line_buff[PWD_BUFFER_SIZE];
    static struct spwd spwd;
    struct spwd *result;

    LOCK;
    if ((ret=getspnam_r(name, &spwd, line_buff,  sizeof(line_buff), &result)) == 0) {
	UNLOCK;
	return &spwd;
    }
    UNLOCK;
    __set_errno(ret);
    return NULL;
}

