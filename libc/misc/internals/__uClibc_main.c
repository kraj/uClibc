/*
 * Manuel Novoa III           Feb 2001
 * Erik Anderseni             Mar 2002
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
#include <stdlib.h>
#include <unistd.h>

#if !defined HAVE_ELF || !defined __UCLIBC_HAS_MMU__
# undef weak_function
# undef weak_const_function
# define weak_function
# define weak_const_function
# define __USE_WEAK_ALIASES
#endif

/*
 * Prototypes.
 */
extern int main(int argc, char **argv, char **envp);
extern int weak_function atexit(void (*function)(void));
extern void weak_function _init(void);
extern void weak_function _fini(void);
extern void weak_function _stdio_init(void);
extern void weak_function _stdio_term(void);
extern int *weak_const_function __errno_location(void);
extern int *weak_const_function __h_errno_location(void);

/*
 * Declare the __environ global variable and create a weak alias environ.
 * Note: Apparently we must initialize __environ for the weak environ
 * symbol to be included.
 */

char **__environ = 0;
weak_alias(__environ, environ);


/*
 * Now for our main routine.
 */
void __attribute__ ((__noreturn__)) 
__uClibc_main(int argc, char **argv, char **envp) 
{
	/* 
	 * Initialize the global variable __environ.
	 */
	__environ = envp;

#if 0
	/* Some security at this point.  Prevent starting a SUID binary
	 * where the standard file descriptors are not opened.  We have
	 * to do this only for statically linked applications since
	 * otherwise the dynamic loader did the work already.  */
	if (unlikely (__libc_enable_secure))
	    __libc_check_standard_fds ();
#endif
	/*
	 * Initialize stdio here.  In the static library case, this will
	 * be bypassed if not needed because of the weak alias above.
	 */
	if (likely(_stdio_init))
	  _stdio_init();

	/* Arrange for dtors to run at exit.  */
	if (_fini && atexit) {
	    atexit (&_fini);
	}
	/* Run all ctors now.  */
	if (_init)
	    _init();

	/*
	 * Note: It is possible that any initialization done above could
	 * have resulted in errno being set nonzero, so set it to 0 before
	 * we call main.
	 */
	if (likely(__errno_location))
	    *(__errno_location()) = 0;

	/* Set h_errno to 0 as well */
	if (likely(__h_errno_location))
	    *(__h_errno_location()) = 0;

	/*
	 * Finally, invoke application's main and then exit.
	 */
	exit(main(argc, argv, envp));
}

#ifdef __USE_WEAK_ALIASES
/*
 * Define an empty function and use it as a weak alias for the stdio
 * initialization routine.  That way we don't pull in all the stdio
 * code unless we need to.  Similarly, do the same for _stdio_term
 * so as not to include atexit unnecessarily.
 *
 * NOTE!!! This is only true for the _static_ case!!!
 */

weak_alias(__environ, environ);
void __uClibc_empty_func(void)
{
}
weak_alias(__uClibc_empty_func, atexit);
weak_alias(__uClibc_empty_func, _init);
weak_alias(__uClibc_empty_func, _fini);
weak_alias(__uClibc_empty_func, __errno_location);
weak_alias(__uClibc_empty_func, __h_errno_location);
weak_alias(__uClibc_empty_func, _stdio_init);
weak_alias(__uClibc_empty_func, _stdio_term);
#endif	
