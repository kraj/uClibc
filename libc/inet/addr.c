/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

/*
 * Manuel Novoa III       Dec 2000
 *
 * Converted to use my new (un)signed long (long) to string routines, which
 * are smaller than the previous functions and don't require static buffers.
 * In the process, removed the reference to strcat and cut object size of
 * inet_ntoa in half (from 190 bytes down to 94).
 */

#define __FORCE_GLIBC
#include <features.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>

int inet_aton(const char *cp, struct in_addr *inp);

#ifdef L_inet_aton
int inet_aton(cp, inp)
const char *cp;
struct in_addr *inp;
{
	unsigned long addr;
	int value;
	int part;

	if (!inp)
		return 0;

	addr = 0;
	for (part = 1; part <= 4; part++) {

		if (!isdigit(*cp))
			return 0;

		value = 0;
		while (isdigit(*cp)) {
			value *= 10;
			value += *cp++ - '0';
			if (value > 255)
				return 0;
		}

		if (*cp++ != ((part == 4) ? '\0' : '.'))
			return 0;

		addr <<= 8;
		addr |= value;
	}

	inp->s_addr = htonl(addr);

	return 1;
}
#endif

#ifdef L_inet_addr
unsigned long inet_addr(cp)
const char *cp;
{
	struct in_addr a;

	if (!inet_aton(cp, &a))
		return -1;
	else
		return a.s_addr;
}
#endif

#ifdef L_inet_ntoa

#include <limits.h>

#if (ULONG_MAX >> 32)
/* We're set up for 32 bit unsigned longs */
#error need to check size allocation for static buffer 'buf'
#endif

extern char *__ultostr(char *buf, unsigned long uval, int base, int uppercase);

char *inet_ntoa(in)
struct in_addr in;
{
	static char buf[16];		/* max 12 digits + 3 '.'s + 1 nul */

	unsigned long addr = ntohl(in.s_addr);
	int i;
	char *p, *q;
   
	q = 0;
	p = buf + sizeof(buf) - 1;
	for (i=0 ; i < 4 ; i++ ) {
		p = __ultostr(p, addr & 0xff, 10, 0 ) - 1;
		addr >>= 8;
		if (q) {
			*q = '.';
		}
		q = p;
	}

	return p+1;
}
#endif
