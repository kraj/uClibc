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
 * Also appended a test routine ( -DSTRSIGNAL_TEST ) to allow a quick check
 * on the buffer length when the sys_errorlist is modified.
 */

#include <string.h>
#include <malloc.h>
#include <signal.h>

extern char *__ltostr(char *buf, long uval, int base, int uppercase);

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

#include <limits.h>

#if (INT_MAX >> 31)
/* We're set up for 32 bit ints */
#error need to check size allocation for static buffer 'retbuf'
#endif

/********************** Function strsignal ************************************/

char *strsignal(int sig)
{
	static char retbuf[28];		/* 28 is sufficient for 32 bit ints */
	static const char unknown_signal[] = "Unknown Signal:";
	char *pos;

	/* if ((sig >= 0) && (sig < _NSIG)) { */
	if ((sig >= 0) && (sig < 32)) { /* WARNING!!! NOT ALL _NSIG DEFINED!!! */
		strcpy(retbuf, sys_siglist[sig]);
		return retbuf;
	}

	pos = __ltostr(retbuf + sizeof(unknown_signal) + 1, sig, 10, 0)
		- sizeof(unknown_signal);
	strcpy(pos, unknown_signal);
	*(pos + sizeof(unknown_signal) - 1) = ' ';
	return pos;
}

/********************** THE END ********************************************/

#if STRSIGNAL_TEST
/* quick way to check for sufficient buffer length */
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
	int max = 0;
	int i, j;
	const char *p;

	printf("_NSIG = %d  from headers\n", _NSIG);
	for ( i=0 ; i < _NSIG ; i++ ) {
		p = sys_siglist[i];
		if (!p) {
			printf("Warning! I only count %d signals!\n", i);
			break;
		}
		j = strlen(sys_siglist[i])+1;
		if (j > max) max = j;
	}
	printf("max len = %i\n", j);

	p = strsignal(INT_MIN);
	printf("<%s>  %d\n", p, strlen(p)+1);

	p = strsignal(i-1);
	printf("last signal %d is %s\n", i-1, p);
	return EXIT_SUCCESS;
}
#endif
