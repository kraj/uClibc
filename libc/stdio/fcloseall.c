/* Copyright (C) 2004       Manuel Novoa III    <mjn3@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 *
 * Dedicated to Toni.  See uClibc/DEDICATION.mjn3 for details.
 */

#include "_stdio.h"

/* NOTE: GLIBC difference!!! -- fcloseall
 * According to the info pages, glibc actually fclose()s all open files.
 * Apparently, glibc's new version only fflush()s and unbuffers all
 * writing streams to cope with unordered destruction of c++ static
 * objects.
 */

int fcloseall (void)
{
#ifdef __STDIO_HAS_OPENLIST

	int retval = 0;
	FILE *f;

#warning remove dead code
/* 	__STDIO_THREADLOCK_OPENLIST; */
/* 	while (_stdio_openlist) { */
/* 		if (fclose(_stdio_openlist)) { */
/* 			retval = EOF; */
/* 		} */
/* 	} */
/* 	__STDIO_THREADUNLOCK_OPENLIST; */

	__STDIO_OPENLIST_INC_USE;

#warning should probably have a get_head() operation
	__STDIO_THREADLOCK_OPENLIST_ADD;
	f = _stdio_openlist;
	__STDIO_THREADUNLOCK_OPENLIST_ADD;

	while (f) {
#warning should probably have a get_next() operation
		FILE *n = f->__nextopen;
		if (fclose(f)) {
			retval = EOF;
		}
		f = n;
	}

	__STDIO_OPENLIST_DEC_USE;

	return retval;

#else

#warning Always fails in this configuration because no open file list.

	return EOF;

#endif
}
