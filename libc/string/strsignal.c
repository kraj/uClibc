/* vi: set sw=4 ts=4: */
/* Copyright (C) 2000 Erik Andersen <andersen@lineo.com>, <andersee@debian.org>
 * This file is part of the uC-Linux and is distributed under the 
 * GNU Library General Public License.
 */

#include <string.h>
#include <malloc.h>
#include <signal.h>

extern __const char *__const _sys_siglist[_NSIG];
extern __const char *__const sys_siglist[_NSIG];

/********************** Function strsignal ************************************/

char *strsignal (int sig)
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
