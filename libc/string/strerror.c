/* Copyright (C) 1991, 1993, 1994 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <string.h>
#include <errno.h>

extern char *itoa(int);

/* Return a string descibing the errno code in ERRNUM.
   The storage is good only until the next call to strerror.
   Writing to the storage causes undefined behavior.  */
char *strerror(int err)
{
	static char retbuf[80];

	if (sys_nerr) {
		if (err < 0 || err >= sys_nerr)
			goto unknown;
		strcpy(retbuf, sys_errlist[err]);
		return retbuf;
	}

	if (err <= 0)
		goto unknown;

  unknown:
	strcpy(retbuf, "Unknown Error: errno=");
	strcat(retbuf, (char *) itoa(err));
	return retbuf;
}
