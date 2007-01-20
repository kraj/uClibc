/* vi: set sw=4 ts=4: */
/*
 * Test application for functions defined in ctype.h
 * Copyright (C) 2000-2006 by Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include "../testsuite.h"

int got_abort;

void aborthandler(int junk)
{
	got_abort = 1;
}

int main(int argc, char *argv[])
{
	signal(SIGABRT, aborthandler);

	init_testsuite("Testing functions defined in assert.h:\n\t");

	got_abort=0;
	assert(0 == 0);
	TEST_NUMERIC(got_abort, 0);

#ifndef NDEBUG
# define NDEBUG
#endif
	got_abort = 0;
	printf("Don't worry -- This next test is supposed to print an assert message:\n");
	fprintf(stderr, "\t");
	assert(0 == 1);
	TEST_NUMERIC(got_abort, 0);

#undef NDEBUG
	got_abort = 0;
	assert(0 == 1);
	TEST_NUMERIC(got_abort, 1);

	exit(0);
}
