/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

#include <string.h>
#include <ctype.h>

int strcasecmp (const char *a, const char *b)
{
    register int n;

    while (*a == *b || (n = tolower (*a) - tolower (*b)) == 0)
    {
	if (*a == '\0')
	    return 0;
	a++, b++;
    }
    return n;
}

