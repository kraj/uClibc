/* Error handler for noninteractive utilities
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */
/* Written by David MacKenzie <djm@gnu.ai.mit.edu>.  */
/* Adjusted slightly by Erik Andersen <andersen@uclibc.org> */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>

libc_hidden_proto(strcmp)
libc_hidden_proto(strerror)
libc_hidden_proto(fprintf)
libc_hidden_proto(exit)
libc_hidden_proto(putc)
libc_hidden_proto(vfprintf)
libc_hidden_proto(fflush)
libc_hidden_proto(fputc)
libc_hidden_proto(__fputc_unlocked)
libc_hidden_proto(stdout)
libc_hidden_proto(stderr)

/* This variable is incremented each time `error' is called.  */
unsigned int error_message_count = 0;
/* Sometimes we want to have at most one error per line.  This
   variable controls whether this mode is selected or not.  */
int error_one_per_line;
/* If NULL, error will flush stdout, then print on stderr the program
   name, a colon and a space.  Otherwise, error will call this
   function without parameters instead.  */
/* void (*error_print_progname) (void) = NULL; */

extern __typeof(error) __error attribute_hidden;
void __error (int status, int errnum, const char *message, ...)
{
    va_list args;

    fflush (stdout);

    va_start (args, message);
    vfprintf (stderr, message, args);
    va_end (args);
    ++error_message_count;
    if (errnum) {
	fprintf (stderr, ": %s", strerror (errnum));
    }
    putc ('\n', stderr);
    if (status)
	exit (status);
}

extern __typeof(error_at_line) __error_at_line attribute_hidden;
void __error_at_line (int status, int errnum, const char *file_name,
	       unsigned int line_number, const char *message, ...)
{
    va_list args;

    if (error_one_per_line) {
	static const char *old_file_name;
	static unsigned int old_line_number;

	if (old_line_number == line_number &&
		(file_name == old_file_name || !strcmp (old_file_name, file_name)))
	    /* Simply return and print nothing.  */
	    return;

	old_file_name = file_name;
	old_line_number = line_number;
    }

    fflush (stdout);

    if (file_name != NULL)
	fprintf (stderr, "%s:%d: ", file_name, line_number);

    va_start (args, message);
    vfprintf (stderr, message, args);
    va_end (args);

    ++error_message_count;
    if (errnum) {
	fprintf (stderr, ": %s", strerror (errnum));
    }
    putc ('\n', stderr);
    if (status)
	exit (status);
}

/* psm: keep this weak, too many use this in common code */
weak_alias(__error,error)
strong_alias(__error_at_line,error_at_line)
