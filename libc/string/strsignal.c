/* vi: set sw=4 ts=4: */
/* Copyright (C) 2000 Erik Andersen <andersen@lineo.com>, <andersee@debian.org>
 * This file is part of the uClinux and is distributed under the 
 * GNU Library General Public License.
 */

/*
 * Manuel Novoa III       Dec 2000
 *
 * Converted to use my new (un)signed long (long) to string routines, which
 * are smaller than the previous functions and don't require static buffers.
 * Removed dependence on strcat in the process.
 * 
 * Also fixed a bug in the signal name lookup code.  While the table is
 * declared with dimension > 60, there are currently on 32 signals listed.
 *
 * Also appended a test routine ( -DCHECK_BUF ) to allow a quick check
 * on the buffer length and the number of known signals when the sys_errorlist
 * is modified.
 *
 * Added the option WANT_SIGLIST for low-memory applications to omit the
 * signal message strings and only output the signal number.
 */

#define WANT_SIGLIST       1

#include <string.h>
#include <malloc.h>
#include <signal.h>

#include <limits.h>

#if (INT_MAX >> 31)
/* We're set up for 32 bit ints */
#error need to check size allocation for static buffer 'retbuf'
#endif

extern char *__ltostr(char *buf, long uval, int base, int uppercase);

#if WANT_SIGLIST

const char *const sys_siglist[] = {
	"Unknown signal",
	"Hangup",
	"Interrupt",
	"Quit",
	"Illegal instruction",
	"Trace/breakpoint trap",
	"IOT trap/Abort",
	"Bus error",
	"Floating point exception",
	"Killed",
	"User defined signal 1",
	"Segmentation fault",
	"User defined signal 2",
	"Broken pipe",
	"Alarm clock",
	"Terminated",
	"Stack fault",
	"Child exited",
	"Continued",
	"Stopped (signal)",
	"Stopped",
	"Stopped (tty input)",
	"Stopped (tty output)",
	"Urgent condition",
	"CPU time limit exceeded",
	"File size limit exceeded",
	"Virtual time alarm",
	"Profile signal",
	"Window size changed",
	"Possible I/O",
	"Power failure",
	"Unused signal",
	NULL
};

#endif

#define NUM_KNOWN_SIGNALS    32

/********************** Function strsignal ************************************/

static char retbuf[28];			/* 28 is sufficient for 32 bit ints */
static const char unknown_signal[] = "Unknown Signal:";

char *strsignal(int sig)
{
	char *pos;

#ifdef WANT_SIGLIST
	/* if ((sig >= 0) && (sig < _NSIG)) { */
	/* WARNING!!! NOT ALL _NSIG DEFINED!!! */
	if ((sig >= 0) && (sig < NUM_KNOWN_SIGNALS)) {
		strcpy(retbuf, sys_siglist[sig]);
		return retbuf;
	}
#endif

	pos = __ltostr(retbuf + sizeof(unknown_signal) + 1, sig, 10, 0)
		- sizeof(unknown_signal);
	strcpy(pos, unknown_signal);
	*(pos + sizeof(unknown_signal) - 1) = ' ';
	return pos;
}

/********************** THE END ********************************************/

#ifdef CHECK_BUF
/* quick way to check for sufficient buffer length */
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
	int max = 0;
	int j, retcode;

	const char *p;
#if WANT_SIGLIST
	int i;
#endif
	retcode = EXIT_SUCCESS;

#if WANT_SIGLIST
	printf("_NSIG = %d  from headers\n", _NSIG);
	for ( i=0 ; sys_siglist[i] ; i++ ) {
		j = strlen(sys_siglist[i])+1;
		if (j > max) max = j;
	}
	if (i != NUM_KNOWN_SIGNALS) {
		printf("Error: strsignal.c - NUM_KNOWN_SIGNALS should be %d\n", i);
		retcode = EXIT_FAILURE;
	}
#endif

	p = strsignal(INT_MIN);
	j = strlen(p)+1;
	if (j > max) max = j;
	printf("strsignal.c - Test of INT_MIN: <%s>  %d\n", p, j);

	if (sizeof(retbuf) != max) {
		printf("Error: strsignal.c - dimension of retbuf should be = %d\n", max);
		retcode = EXIT_FAILURE;
	}
	printf("strsignal.c - dimension of retbuf correct at %d\n", max);

	return retcode;
}
#endif
