/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

/*
 * Dec 2000          Manuel Novoa III
 *
 *   Made atexit handling conform to standards... i.e. no args.
 *   Removed on_exit since it did not match gnu libc definition.
 *   Combined atexit and __do_exit into one object file.
 *
 * Feb 2000          Manuel Novoa III
 *
 *   Reworked file after addition of __uClibc_main.
 *   Changed name of __do_exit to atexit_handler.
 *   Changed name of __cleanup to __uClibc_cleanup.
 *   Moved declaration of __uClibc_cleanup to __uClibc_main
 *      where it is initialized with (possibly weak alias)
 *      __stdio_close_all.
 */

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

typedef void (*vfuncp) (void);
extern vfuncp __uClibc_cleanup;

#ifdef L_atexit
extern void __stdio_close_all(void);

static vfuncp __atexit_table[__UCLIBC_MAX_ATEXIT];
static int __atexit_count = 0;

static void atexit_handler(void)
{
	int count;

	/*
	 * Guard against more functions being added and againt being reinvoked.
	 */
	__uClibc_cleanup = 0;

	/* In reverse order */
	for (count = __atexit_count ; count-- ; ) {
		(*__atexit_table[count])();
	}
	if (__stdio_close_all)
	  __stdio_close_all();
}

int atexit(vfuncp ptr)
{
	if ((__uClibc_cleanup == 0) || (__atexit_count >= __UCLIBC_MAX_ATEXIT)) {
		__set_errno(ENOMEM);
		return -1;
	}
	if (ptr) {
		__uClibc_cleanup = atexit_handler;
		__atexit_table[__atexit_count++] = ptr;
	}
	return 0;
}
#endif

#ifdef L_exit
void exit(int rv)
{
	if (__uClibc_cleanup) {		/* Not already executing __uClibc_cleanup. */
		__uClibc_cleanup();
	}
	_exit(rv);
}
#endif
