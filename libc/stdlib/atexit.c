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
 * Feb 2001          Manuel Novoa III
 *
 *   Reworked file after addition of __uClibc_main.
 *   Changed name of __do_exit to atexit_handler.
 *   Changed name of __cleanup to __uClibc_cleanup.
 *   Moved declaration of __uClibc_cleanup to __uClibc_main
 *      where it is initialized with (possibly weak alias)
 *      __stdio_flush_buffers.
 *
 * Jul 2001          Steve Thayer
 * 
 *   Added an on_exit implementation (that now matches gnu libc definition.)
 *   Pulled atexit_handler out of the atexit object since it is now required by
 *   on_exit as well.  Renamed it to __exit_handler.
 *   Fixed a problem where exit functions stop getting called if one of
 *   them calls exit().
 *   As a side effect of these changes, abort() no longer calls the exit
 *   functions (it now matches the gnu libc definition).
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define __MAX_EXIT __UCLIBC_MAX_ATEXIT

typedef void (*aefuncp) (void);         /* atexit function pointer */
typedef void (*oefuncp) (int, void *);  /* on_exit function pointer */
typedef enum {
	ef_atexit,
	ef_on_exit
} ef_type; /* exit function types */

extern void __stdio_flush_buffers(void);

/* this is in the L_exit object */
extern void (*__exit_cleanup) (int);

/* these are in the L___do_exit object */
extern int __exit_count;
extern void __exit_handler(int);
extern struct exit_function {
	ef_type type;	/* ef_atexit or ef_on_exit */
	union {
		aefuncp atexit;
		struct {
			oefuncp func;
			void *arg;
		} on_exit;
	} funcs;
} __exit_function_table[__MAX_EXIT];

#ifdef L_atexit
	/*
 * register a function to be called at normal program termination
 * (the registered function takes no arguments)
	 */
int atexit(aefuncp func)
{
	struct exit_function *efp;

	if (__exit_count >= __MAX_EXIT) {
		__set_errno(ENOMEM);
		return -1;
	}
	if (func) {
		__exit_cleanup = __exit_handler; /* enable cleanup */
		efp = &__exit_function_table[__exit_count++];
		efp->type = ef_atexit;
		efp->funcs.atexit = func;
	}
	return 0;
}
#endif

#ifdef L_on_exit
/*
 * register a function to be called at normal program termination
 * the registered function takes two arguments:
 *     status - the exit status that was passed to the exit() function
 *     arg - generic argument
 */
int on_exit(oefuncp func, void *arg)
{
	struct exit_function *efp;

	if (__exit_count >= __MAX_EXIT) {
		__set_errno(ENOMEM);
		return -1;
	}
	if (func) {
		__exit_cleanup = __exit_handler; /* enable cleanup */
		efp = &__exit_function_table[__exit_count++];
		efp->type = ef_on_exit;
		efp->funcs.on_exit.func = func;
		efp->funcs.on_exit.arg = arg;
	}
	return 0;
}
#endif

#ifdef L___exit_handler
struct exit_function __exit_function_table[__MAX_EXIT];
int __exit_count = 0; /* Number of registered exit functions */

/*
 * Handle the work of executing the registered exit functions
 */
void __exit_handler(int status)
{
	struct exit_function *efp;

	/* In reverse order */
	for ( ; __exit_count-- ; ) {
		efp = &__exit_function_table[__exit_count];
		switch (efp->type) {
		case ef_on_exit:
			if (efp->funcs.on_exit.func) {
				(efp->funcs.on_exit.func) (status, efp->funcs.on_exit.arg);
			}
			break;
		case ef_atexit:
			if (efp->funcs.atexit) {
				(efp->funcs.atexit) ();
			}
			break;
		}
	}
}
#endif

#ifdef L_exit
extern void __stdio_flush_buffers(void);
void (*__exit_cleanup) (int) = 0;

/*
 * Normal program termination
 */
void exit(int rv)
{
	/* Perform exit-specific cleanup (atexit and on_exit) */
	if (__exit_cleanup) {
		__exit_cleanup(rv);
	}

	/* Clean up everything else */
	if (__stdio_flush_buffers) 
	    __stdio_flush_buffers();

	_exit(rv);
}
#endif
