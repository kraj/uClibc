/* $Id: crt0.c,v 1.1 2002/09/16 08:08:33 tobiasa Exp $ */

/* C base for Linux/CRIS 2.0/2.4
 */

//#define DEBUG

/* The first piece of initialized data.  */
int __data_start = 0;

/* N.B.: It is important that this be the first function.
   This file is the first thing in the text section.  */

void
_start ()
{
	/* on the stack we have argc. we can calculate argv/envp
	 * from that and the succeeding stack location, but fix so
	 * we get the right calling convention (regs in r10/r11)
	 *
	 * to understand this you really ought to read fs/binfmt_elf.c
	 */
	__asm__ volatile("pop $r10");
	__asm__ volatile("move.d $sp, $r11");
	__asm__ volatile("jump start1");
}

void __uClibc_main(int argc, char **argv, char **envp)
         __attribute__ ((__noreturn__));

static void
start1 (int argc, char **argv)
{
	char** environ;

	/* The environment starts just after ARGV.  */
	environ = &argv[argc + 1];
	
	/* If the first thing after ARGV is the arguments
	   themselves, there is no environment.  */
	if ((char *) environ == *argv)
		/* The environment is empty.  Make environ
		   point at ARGV[ARGC], which is NULL.  */
		--environ;
	
	/* Leave control to the libc */

	__uClibc_main(argc, argv, environ);
}
