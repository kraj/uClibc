/*
 * Manuel Novoa III           Feb 2001
 * Erik Andersen              Mar 2002
 *
 * __uClibc_main is the routine to be called by all the arch-specific
 * versions of crt0.S in uClibc.
 *
 * It is meant to handle any special initialization needed by the library
 * such as setting the global variable(s) __environ (environ) and
 * initializing the stdio package.  Using weak symbols, the latter is
 * avoided in the static library case.
 */

#define	_ERRNO_H
#include <unistd.h>
#include <stdlib.h>


/*
 * Prototypes.
 */
extern int  main(int argc, char **argv, char **envp);
extern void weak_function _init(void);
extern void weak_function _fini(void);
extern void weak_function _stdio_init(void);
extern int *weak_const_function __errno_location(void);
extern int *weak_const_function __h_errno_location(void);
extern int weak_function atexit(void (*function)(void));
#ifdef __UCLIBC_HAS_LOCALE__
extern void weak_function _locale_init(void);
#endif
#ifdef __UCLIBC_HAS_THREADS__
extern void weak_function __pthread_initialize_minimal(void);
#endif




/*
 * Declare the __environ global variable and create a weak alias environ.
 * Note: Apparently we must initialize __environ for the weak environ
 * symbol to be included.
 */

char **__environ = 0;
weak_alias(__environ, environ);




void __attribute__ ((__noreturn__)) 
__uClibc_main(int argc, char **argv, char **envp) 
{
	/* If we are dynamically linked the shared lib loader
	 * already did this for us.  But if we are statically
	 * linked, we need to do this for ourselves. */
	if (__environ==NULL) {
		/* Statically linked. */ 
		__environ = envp;
	}

#ifdef __UCLIBC_HAS_THREADS__
	if (likely(__pthread_initialize_minimal!=NULL))
	    __pthread_initialize_minimal();
#endif

#if 0
	/* Some security at this point.  Prevent starting a SUID binary
	 * where the standard file descriptors are not opened.  We have
	 * to do this only for statically linked applications since
	 * otherwise the dynamic loader did the work already.  */
	if (unlikely (__libc_enable_secure!=NULL))
		__libc_check_standard_fds ();
#endif

#ifdef __UCLIBC_HAS_LOCALE__
	/* Initialize the global locale structure. */
	if (likely(_locale_init!=NULL))
	    _locale_init();
#endif

	/*
	 * Initialize stdio here.  In the static library case, this will
	 * be bypassed if not needed because of the weak alias above.
	 */
	if (likely(_stdio_init != NULL))
		_stdio_init();

	/* Arrange for dtors to run at exit.  */
	if (likely(_fini!=NULL && atexit)) {
		atexit (&_fini);
	}

	/* Run all ctors now.  */
	if (likely(_init!=NULL))
		_init();

	/*
	 * Note: It is possible that any initialization done above could
	 * have resulted in errno being set nonzero, so set it to 0 before
	 * we call main.
	 */
	if (likely(__errno_location!=NULL))
		*(__errno_location()) = 0;

	/* Set h_errno to 0 as well */
	if (likely(__h_errno_location!=NULL))
		*(__h_errno_location()) = 0;

	/*
	 * Finally, invoke application's main and then exit.
	 */
	exit(main(argc, argv, envp));
}
