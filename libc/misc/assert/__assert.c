/*  Copyright (C) 2002     Manuel Novoa III
 *  An __assert() function compatible with the modified glibc assert.h
 *  that is used by uClibc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#define _STDIO_UTILITY	/* For _stdio_fdout and _int10tostr. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Get the prototype from assert.h as a double-check. */
#undef NDEBUG
#include <assert.h>
#undef assert

void __assert(const char *assertion, const char * filename,
			  int linenumber, const char * function)
{
	char buf[__BUFLEN_INT10TOSTR];

	_stdio_fdout(STDERR_FILENO,
#if 0
				 program_name,	/* TODO: support program_name like glibc? */
				 ": ",
#endif
				 filename,
				 ":",
				 _int10tostr(buf+sizeof(buf)-1, linenumber),
				 ": ",
				 /* Function name isn't available with some compilers. */
				 ((function == NULL) ? "?function?" : function),
				 ":  Assertion `",
				 assertion,
				 "' failed.\n",
				 NULL
				 );
	abort();
}
