/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

/*
 * Manuel Novoa III       Dec 2000
 *
 * Modifications:
 *   Made atexit handling conform to standards... i.e. no args.
 *   Removed on_exit since it did not match gnu libc definition.
 *   Combined atexit and __do_exit into one object file.
 */

#include <stdlib.h>
#include <errno.h>

typedef void (*vfuncp) (void);
extern vfuncp __cleanup;

#ifdef L_atexit
static vfuncp __atexit_table[__UCLIBC_MAX_ATEXIT];
static int __atexit_count = 0;

static void __do_exit(void)
{
	int count = __atexit_count - 1;

	__atexit_count = -1;		/* ensure no more will be added */
	__cleanup = 0;				/* Calling exit won't re-do this */

	/* In reverse order */
	for (; count >= 0; count--) {
		(*__atexit_table[count])();
	}
}

int atexit(vfuncp ptr)
{
	if ((__atexit_count < 0) || (__atexit_count >= __UCLIBC_MAX_ATEXIT)) {
		errno = ENOMEM;
		return -1;
	}
	if (ptr) {
		__cleanup = __do_exit;
		__atexit_table[__atexit_count++] = ptr;
	}
	return 0;
}
#endif

#ifdef L_exit
vfuncp __cleanup = 0;

void exit(int rv)
{
	if (__cleanup)
		__cleanup();
	_exit(rv);
}
#endif
