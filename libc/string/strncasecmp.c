/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

#include <string.h>
#include <ctype.h>

int strncasecmp (const char *a, const char *b, size_t len)
{
    register int n;

    if (len < 1)
	return 0;
    while (*a == *b || (n = tolower (*a) - tolower (*b)) == 0)
    {
	if (*a == '\0' || --len < 1)
	    return 0;
	a++, b++;
    }
    return n;
}

