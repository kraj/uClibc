/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

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

extern char *itoa(int);

char *inet_ntoa(in)
struct in_addr in;
{
	static char buf[18];
	unsigned long addr = ntohl(in.s_addr);

	strcpy(buf, itoa((addr >> 24) & 0xff));
	strcat(buf, ".");
	strcat(buf, itoa((addr >> 16) & 0xff));
	strcat(buf, ".");
	strcat(buf, itoa((addr >> 8) & 0xff));
	strcat(buf, ".");
	strcat(buf, itoa(addr & 0xff));

	return buf;
}
#endif
