/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>

char *getenv(const char *var)
{
    char **p;
    int len;

    len = strlen(var);

    if (!__environ)
	return NULL;

    for (p = __environ; *p; p++) {
	if (memcmp(var, *p, len) == 0 && (*p)[len] == '=')
	    return *p + len + 1;
    }
    return NULL;
}
