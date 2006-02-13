/* Copyright (C) 2004       Manuel Novoa III    <mjn3@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 *
 * Dedicated to Toni.  See uClibc/DEDICATION.mjn3 for details.
 */

#include <features.h>

#ifdef __USE_GNU
#include "_stdio.h"

libc_hidden_proto(fclose)

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

	__STDIO_THREADLOCK_OPENLIST;
	while (_stdio_openlist) {
		if (fclose(_stdio_openlist)) {
			retval = EOF;
		}
	}
	__STDIO_THREADUNLOCK_OPENLIST;

	return retval;

#else

#warning Always fails in this configuration because no open file list.

	return EOF;

#endif
}
#endif
