/* vi: set sw=4 ts=4: */
/*
 * alarm() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#define setitimer __setitimer

#include "syscalls.h"
#include <unistd.h>
#ifdef __NR_alarm
#define __NR___alarm __NR_alarm
attribute_hidden _syscall1(unsigned int, __alarm, unsigned int, seconds);
#else
#include <sys/time.h>
unsigned int attribute_hidden __alarm(unsigned int seconds)
{
	struct itimerval old, new;
	unsigned int retval;

	new.it_value.tv_usec = 0;
	new.it_interval.tv_sec = 0;
	new.it_interval.tv_usec = 0;
	new.it_value.tv_sec = (long int) seconds;
	if (setitimer(ITIMER_REAL, &new, &old) < 0) {
		return 0;
	}
	retval = old.it_value.tv_sec;
	if (old.it_value.tv_usec) {
		++retval;
	}
	return retval;
}
#endif
strong_alias(__alarm,alarm)
