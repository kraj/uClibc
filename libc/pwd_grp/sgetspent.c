/*
 * sgetspent.c
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

#include <errno.h>
#include <stdio.h>
#include "config.h"

int sgetspent_r (const char *string, struct spwd *spwd,
	char *buff, size_t buflen, struct spwd **crap)
{
    return(__sgetspent_r(string, spwd, buff, buflen));
}

struct spwd *sgetspent(const char *string)
{
    static char line_buff[PWD_BUFFER_SIZE];
    static struct spwd spwd;

    if (sgetspent_r(string, &spwd, line_buff, sizeof(line_buff), NULL) != -1) {
	return &spwd;
    }
    return NULL;
}
