/* vi: set sw=4 ts=4: */
/* syscall for blackfin/uClibc
 *
 * Copyright (C) 2004 by Analog Devices Inc.
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <features.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>

long syscall(long sysnum, long a, long b, long c, long d, long e)
{
	int _r0 = 0;
    asm volatile(
	    "p0 = %1;"		/*SysCall Number*/
	    "r0 = %2;"
	    "r1 = %3;"
	    "r2 = %4;"
		"r3 = %6;"
		"r4 = %5;"
		"excpt 0;"		/*Call the System Call*/
		"%0 = r0;"		/*Store the result of syscall*/
	    : "=r"(_r0)
	    : "r"(sysnum), "r"(a), "r"(b),
	      "r"(c), "r"(d), "r"(e)
	    : "memory");

    if(_r0 >=(unsigned long) -4095) {
	(*__errno_location())=(-_r0);
	_r0=(unsigned long) -1;
    }
    return (long) _r0;
}
