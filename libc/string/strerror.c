/* Copyright (C) 1991, 1993, 1994 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

/*
 * Manuel Novoa III       Dec 2000
 *
 * Converted to use my new (un)signed long (long) to string routines, which
 * are smaller than the previous functions and don't require static buffers.
 * Removed dependence on strcat in the process.
 *
 * Also appended a test routine ( -DCHECK_BUF ) to allow a quick check
 * on the buffer length when the sys_errorlist is modified.
 *
 * Added the option WANT_ERRORLIST for low-memory applications to omit the
 * error message strings and only output the error number.
 *
 * Manuel Novoa III       Feb 2002
 *
 * Change to _int10tostr and fix a bug in end-of-buf arg.
 */

#define WANT_ERRORLIST     1

#define _STDIO_UTILITY			/* For _int10tostr. */
#include <stdio.h>
#include <string.h>
#include <errno.h>

#if WANT_ERRORLIST
static char retbuf[48];
#else
#if __BUFLEN_INT10TOSTR > 12
#error currently set up for 32 bit ints max!
#endif
static char retbuf[33];			/* 33 is sufficient for 32 bit ints */
#endif
static const char unknown_error[] = "Unknown Error: errno"; /* = */

/* Return a string descibing the errno code in ERRNUM.
   The storage is good only until the next call to strerror.
   Writing to the storage causes undefined behavior.  */
char *strerror(int err)
{
	char *pos;

#if WANT_ERRORLIST
	if ((err >= 0) && (err < sys_nerr)) {
		strcpy(retbuf, sys_errlist[err]);
		return retbuf;
	}
#endif

	/* unknown error -- leave space for the '=' */
	pos = _int10tostr(retbuf+sizeof(retbuf)-1, err)	- sizeof(unknown_error);
	strcpy(pos, unknown_error);
	*(pos + sizeof(unknown_error) - 1) = '=';
	return pos;
}

#ifdef CHECK_BUF
/* quick way to check buffer length */
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
	int max = 0;
	int j, retcode;
	char *p;
#if WANT_ERRORLIST
	int i;
#endif

	retcode = EXIT_SUCCESS;

#if WANT_ERRORLIST
	for ( i=0 ; i < sys_nerr ; i++ ) {
		j = strlen(sys_errlist[i])+1;
		if (j > max) max = j;
	}
#endif

	p = strerror(INT_MIN);
	j = retbuf+sizeof(retbuf) - p;
	if ( > max) {
	    max = j;
	    printf("strerror.c - Test of INT_MIN: <%s>  %d\n", p, j);
	}

	if (sizeof(retbuf) != max) {
		printf("Error: strerror.c - dimension of retbuf should be = %d\n", max);
		retcode = EXIT_FAILURE;
	}
	printf("Passed.\n");

	return retcode;
}
#endif
