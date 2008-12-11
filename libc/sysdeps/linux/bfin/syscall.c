/* vi: set sw=4 ts=4: */
/* syscall for blackfin/uClibc
 *
 * Copyright (C) 2004-2006 by Analog Devices Inc.
 * Copyright (C) 2002 by Erik Andersen <andersen@uclibc.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <features.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>

long syscall(long sysnum, long a, long b, long c, long d, long e, long f)
{
	int _r0 = 0;

	__asm__ __volatile__ (
		"excpt 0;"
		: "=q0" (_r0)
		: "qA" (sysnum),
		  "q0" (a),
		  "q1" (b),
		  "q2" (c),
		  "q3" (d),
		  "q4" (e),
		  "q5" (f)
		: "memory", "CC");

	if (_r0 >= (unsigned long) -4095) {
		(*__errno_location()) = (-_r0);
		_r0 = (unsigned long) -1;
	}

	return (long)_r0;
}
