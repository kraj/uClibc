/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

int bcmp(const __ptr_t dest, const __ptr_t src, size_t len)
{
	return memcmp(dest, src, len);
}

