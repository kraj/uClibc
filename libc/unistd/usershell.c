/* vi: set sw=4 ts=4: */
/* getusershell and friends for uClibc
 *
 * Copyright (C) 2002 by Robert Griebl <griebl@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <unistd.h>

/* Rely on the .bss to zero these */
static FILE *fp;
static int isopen;
static char **cursh;
static char *validsh [] = { "/bin/sh", "/bin/csh", 0 };

void endusershell ( void )
{
	if ( fp && isopen )
		fclose ( fp );
	isopen = 0;
}

void setusershell ( void )
{
	if ( isopen )
		endusershell ( );
	fp = fopen ( "/etc/shells", "r" );
	cursh = validsh;
	isopen = 1;
}

char *getusershell ( void )
{
	char line [BUFSIZ];

	if ( !isopen )
		setusershell ( );

	if ( fp ) {
		return fgets ( line, sizeof( line ) - 1, fp );
	} else {
		char *result = *cursh;

		if ( result )
			cursh++;
		return result;  
	}
}

