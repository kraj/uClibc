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

#include <errno.h>

/* ATEXIT.H */

/*
 * NOTE!!! The following should match the value returned by
 *     by sysconf(_SC_ATEXIT_MAX) in unistd/sysconf.c
 */
#define MAXATEXIT 20			/* AIUI Posix requires 10 */

typedef void (*vfuncp) (void);

extern vfuncp __cleanup;
extern void __do_exit();
extern void _exit __P((int __status)) __attribute__ ((__noreturn__));

extern vfuncp __atexit_table[MAXATEXIT];
extern int __atexit_count;

/* End ATEXIT.H */

#ifdef L_atexit
int atexit(vfuncp ptr)
{
	if ((__atexit_count < 0) || (__atexit_count >= MAXATEXIT)) {
		errno = ENOMEM;
		return -1;
	}
	if (ptr) {
		__cleanup = __do_exit;
		__atexit_table[__atexit_count++] = ptr;
	}
	return 0;
}

vfuncp __atexit_table[MAXATEXIT];
int __atexit_count = 0;

void __do_exit(int rv)
{
	int count = __atexit_count - 1;

	__atexit_count = -1;		/* ensure no more will be added */
	__cleanup = 0;				/* Calling exit won't re-do this */

	/* In reverse order */
	for (; count >= 0; count--) {
		(*__atexit_table[count])();
	}
}
#endif

#ifdef L_exit
void __stdio_close_all(void);	/* note: see _start.S - could be faked */

vfuncp __cleanup = 0;

void exit(int rv)
{
	if (__cleanup)
		__cleanup();
	__stdio_close_all();
	_exit(rv);
}
#endif
