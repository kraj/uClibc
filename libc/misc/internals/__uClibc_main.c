/*
 * Manuel Novoa III           Feb 2001
 *
 * __uClibc_main is the routine to be called by all the arch-specific
 * versions of crt0.S in uClibc.
 *
 * It is meant to handle any special initialization needed by the library
 * such as setting the global variable(s) __environ (environ) and
 * initializing the stdio package.  Using weak symbols, the latter is
 * avoided in the static library case.
 */

#include <stdlib.h>
#include <unistd.h>

/*
 * Prototypes.
 */

extern int main(int argc, char **argv, char **envp);

void __uClibc_main(int argc, char **argv, char **envp)
	 __attribute__ ((__noreturn__));

/*
 * Define an empty function and use it as a weak alias for the stdio
 * initialization routine.  That way we don't pull in all the stdio
 * code unless we need to.  Similarly, do the same for __stdio_close_all
 * so as not to include atexit unnecessarily.
 *
 * NOTE!!! This is only true for the _static_ case!!!
 */

void __uClibc_empty_func(void)
{
}

 __attribute__ ((__weak__, __alias__("__uClibc_empty_func")))
void __init_stdio(void);

 __attribute__ ((__weak__, __alias__("__uClibc_empty_func")))
void __stdio_close_all(void);

typedef void (*vfuncp) (void);
vfuncp __uClibc_cleanup = __stdio_close_all;

/*
 * Now for our main routine.
 */

void __uClibc_main(int argc, char **argv, char **envp) 
{
	/* 
	 * Initialize the global variable __environ.
	 */
	__environ = envp;

	/*
	 * Initialize stdio here.  In the static library case, this will
	 * be bypassed if not needed because of the weak alias above.
	 */
	__init_stdio();

	/*
	 * Finally, invoke application's main and then exit.
	 */
	exit(main(argc, argv, envp));
}

/*
 * Declare the __environ global variable and create a weak alias environ.
 */

char **__environ = 0;

__asm__(".weak environ;environ = __environ");




