/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

void bcopy(const __ptr_t src, __ptr_t dest, size_t len)
{
/*   (void) memcpy(dest, src, len); */
	while (len--)
		*(char *) (dest++) = *(char *) (src++);
}

