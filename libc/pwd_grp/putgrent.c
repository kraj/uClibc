/* vi: set sw=4 ts=4: */
/*
 * putgrent.c
 * Copyright (C) 2003 Erik Andersen <andersee@debian.org>
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


/* Write the given entry onto the given stream.  */
int putgrent(const struct group *__restrict grp,
		     FILE *__restrict f)
{
	if (grp == NULL || f == NULL) {
		__set_errno(EINVAL);
		return -1;
	}
	if (fprintf(f, "%s:%s:%u:", grp->gr_name, 
				grp->gr_passwd, grp->gr_gid) < 0) 
	{
		return -1;
	}
	if (grp->gr_mem) {
		int i;
		char **gr_mem = grp->gr_mem;
		while(*gr_mem) {
			if (fprintf(f, (i++)? ",%s" : "%s", *gr_mem) < 0) {
				return -1;
			}
			gr_mem++;
		}
	}

	if (fputc('\n', f) < 0) {
		return -1;
	}
	return 0;
}

