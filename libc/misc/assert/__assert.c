/* Copyright (C) 1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

extern char *itoa(int);

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
	errput(filename);
	errput(":");
	errput(itoa(linenumber));
	errput(function ? ": " : "");
	errput(function ? function : "");
	errput(function ? "() " : "");
	errput(": Assertion \"");
	errput(assertion);
	errput("\" failed.\n");
	abort();
}
