/* vi: set sw=4 ts=4: */
/* Copyright (C) 2000 Erik Andersen <andersen@lineo.com>, <andersee@debian.org>
 * This file is part of the uC-Linux and is distributed under the 
 * GNU Library General Public License.
 */

#include <string.h>
#include <malloc.h>
#include <signal.h>

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

/********************** Function strsignal ************************************/

char *strsignal(int sig)
{
	static char retbuf[80];

	if (sys_siglist) {
		if (sig < 0 || sig >= _NSIG)
			goto unknown;
		strcpy(retbuf, sys_siglist[sig]);
		return retbuf;
	}

	if (sig <= 0)
		goto unknown;

  unknown:
	strcpy(retbuf, "Unknown Signal: ");
	strcat(retbuf, (char *) itoa(sig));
	return retbuf;
}

/********************** THE END ********************************************/
