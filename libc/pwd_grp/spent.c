/*
 * spent.c - Based on pwent.c
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

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <shadow.h>
#include <fcntl.h>

/*
 * setspent(), endspent(), and getspent() are included in the same object
 * file, since one cannot be used without the other two, so it makes sense to
 * link them all in together.
 */

#define PWD_BUFFER_SIZE 256

/* file descriptor for the password file currently open */
static int spwd_fd = -1;

void setspent(void)
{
	if (spwd_fd != -1)
		close(spwd_fd);

	spwd_fd = open(_PATH_SHADOW, O_RDONLY);
}

void endspent(void)
{
	if (spwd_fd != -1)
		close(spwd_fd);
	spwd_fd = -1;
}

int getspent_r (struct spwd *spwd, char *buff, 
	size_t buflen, struct spwd **crap)
{
	if (spwd_fd != -1 && __getspent_r(spwd, buff, buflen, spwd_fd) != -1) {
		return 0;
	}
	return -1;
}

struct spwd *getspent(void)
{
	static char line_buff[PWD_BUFFER_SIZE];
	static struct spwd spwd;

	if (getspent_r(&spwd, line_buff, PWD_BUFFER_SIZE, NULL) != -1) {
		return &spwd;
	}
	return NULL;
}

