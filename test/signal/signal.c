/* Tester for string functions.
   Copyright (C) 1995, 1996, 1997, 1998, 1999 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <signal.h>


/* -------------------------------------------------*/
/* This stuff is common to all the testing routines */
/* -------------------------------------------------*/
const char *it = "<UNSET>";	/* Routine name for message routines. */
size_t errors = 0;

void check (int thing, int number)
{
  if (!thing)
    {
      printf("%s flunked test %d\n", it, number);
      ++errors;
    }
}

void equal (const char *a, const char *b, int number)
{
  check(a != NULL && b != NULL && (strcmp(a, b)==0), number);
}


/* -------------------------------------------------*/
/* Let the tests begin....                          */
/* -------------------------------------------------*/

int global_int = 0;

void set_global_int_to_one(int signum)
{
    printf("entering set_global_int_to_one\n");
    global_int = 1;
    return;
}

void
signal_test_1 (void)
{
  global_int = 0;

  signal(SIGUSR1, set_global_int_to_one);
  raise( SIGUSR1);
  /* This should have first jumped to the signal handler */
  check ( (global_int == 0), 0);

  signal(SIGUSR1, SIG_DFL);
  raise( SIGUSR1);
  printf("Here I am.\n");
}


int
main (void)
{
  int status;

  signal_test_1 ();

  if (errors == 0)
    {
      status = EXIT_SUCCESS;
      printf("No errors.\n");
    }
  else
    {
      status = EXIT_FAILURE;
      printf("%d errors.\n", errors);
    }
  exit(status);
}
