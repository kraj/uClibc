/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

int remove(src)
__const char *src;
{
	extern int errno;
	int er = errno;
	int rv = unlink(src);

	if (rv < 0 && errno == EISDIR)
		rv = rmdir(src);
	if (rv >= 0)
		__set_errno(er);
	return rv;
}

