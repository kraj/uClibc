/* vi: set sw=4 ts=4: */
/*
 * __getgrent.c - This file is part of the libc-8086/grp package for ELKS,
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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "config.h"


/*
 * This is the core group-file read function.  It behaves exactly like
 * getgrent() except that it is passed a file descriptor.  getgrent()
 * is just a wrapper for this function.
 */
int __getgrent_r (struct group *__restrict group, 
	char *__restrict line_buff, size_t buflen, int grp_fd)
{
	char *endptr, *field_begin, **members;
	int i, line_len, member_num = 0;


	if (buflen<GRP_BUFFER_SIZE) {
		return ERANGE;
	}

	if (grp_fd==-1)
		setgrent();

	/* We use the restart label to handle malformatted lines */
restart:
	/* Read the group line into the buffer for processing */
	if ((line_len = read(grp_fd, line_buff, buflen)) <= 0) {
		return EIO;
	}
	field_begin = strchr(line_buff, '\n');
	if (field_begin != NULL)
		lseek(grp_fd, (long) (1 + field_begin - (line_buff + line_len)), SEEK_CUR);
	else {
		/* The line is too long - skip it. :-\ */
		do {
			if ((line_len = read(grp_fd, line_buff, buflen)) <= 0) {
				return EIO;
			}
		} while (!(field_begin = strchr(line_buff, '\n')));
		lseek(grp_fd, (long) (field_begin - line_buff) - line_len + 1, SEEK_CUR);
		goto restart;
	}
	if (*line_buff == '#' || *line_buff == ' ' || *line_buff == '\n' || *line_buff == '\t')
		goto restart;
	*field_begin = '\0';

	/* We've read the line; now parse it. */
	field_begin = line_buff;
	for (i = 0; i < 3; i++) {
		switch (i) {
			case 0:
				group->gr_name = field_begin;
				break;
			case 1:
				group->gr_passwd = field_begin;
				break;
			case 2:
				group->gr_gid = (__gid_t) strtoul(field_begin, &endptr, 10);
				if (*endptr != ':')
					goto restart;
				break;
		}
		if (i < 3) {
			field_begin = strchr(field_begin, ':');
			if (field_begin == NULL)
				break;
			*field_begin++ = '\0';
		}
	}

	members = (char **) malloc(sizeof(char *));
	if (members==NULL) {
		return ENOMEM;
	}
	while(field_begin && strlen(field_begin)) {
		members[member_num++] = field_begin;
		members = (char **) realloc(members, (member_num + 1) * sizeof(char *));
		if (members==NULL) {
			return ENOMEM;
		}
		endptr = strchr(field_begin, ',');
		if (endptr == NULL) {
			/* Final entry */
			break;
		}
		*field_begin++ = '\0';
	}
	members[member_num] = NULL;
	group->gr_mem = members;
	return 0;
}
