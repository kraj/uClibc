/* vi: set sw=4 ts=4: */
/*
 * tmpnam for uClibc
 *
 * Copyright (C) 2000 by David Whedon <dwhedon@gordian.com>
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
 *
 * Modified by Erik Andersen <anderse@debian.org> to be reentrant for
 * the case when S != NULL...
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


static char tmpnam_buffer[L_tmpnam];


/* Generate a unique filename in /tmp */
char * tmpnam (char *s)
{
    int num __attribute__ ((unused));       /* UNINITIALIZED, so we get whatever crap
					     happens to be in memory, producing (in theory)
					     pseudo-random tmpname results... */
    int n2;
    char buf[L_tmpnam], *ptr;
    struct stat statbuf;
    unsigned char l, i;

    ptr=s ?  s : buf;

    l = snprintf(ptr, L_tmpnam, "%s/tmp.", P_tmpdir);

again:
    n2 = num;
    for (i = l ; i < l + 6; i++) {
	ptr[i] = '0' + n2 % 10;
	n2 /= 10;
    }

    if (stat (ptr, &statbuf) == 0){
	num++;
	goto again;
    }

    if (s == NULL)
	return (char *) memcpy (tmpnam_buffer, ptr, L_tmpnam);

    return ptr;
}

