/* Copyright (C) 1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

/*
 * Manuel Novoa III       Dec 2000
 *
 * Converted to use my new (un)signed long (long) to string routines, which
 * are smaller than the previous functions and don't require static buffers.
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#if (INT_MAX >> 31)
/* We're set up for 32 bit ints */
#error need to check size allocation for buffer 'buf'
#endif

extern char *__ltostr(char *buf, unsigned long uval, int base, int uppercase);

static void errput(str)
const char *str;
{
	write(2, str, strlen(str));
}

void __assert(assertion, filename, linenumber, function)
const char *assertion;
const char *filename;
int linenumber;
const char *function;
{
	char buf[12];

	errput(filename);
	errput(":");
	errput(__ltostr(buf + sizeof(buf) - 1, linenumber, 10, 0));
	errput(function ? ": " : "");
	errput(function ? function : "");
	errput(function ? "() " : "");
	errput(": Assertion \"");
	errput(assertion);
	errput("\" failed.\n");
	abort();
}
