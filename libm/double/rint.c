/* vi: set sw=4 ts=4: */
/*
 * rint for uClibc
 *
 * Copyright (C) 2001 by Lineo, inc.  
 * Written by Erik Andersen <andersen@lineo.com>, <andersee@debian.org>
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
 *
 */

#include <math.h>

/* From the Linux man page:
 *
 * NAME
 *	rint - round to closest integer
 *
 * SYNOPSIS
 *	#include <math.h>
 *	double rint(double x);
 *
 * DESCRIPTION
 *	The rint() function rounds x to an integer value according
 *	to the prevalent rounding mode.  The default rounding mode
 *	is to round to the nearest integer.  
 *
 * RETURN VALUE
 *	The rint() function returns the integer value as a float­ 
 *	ing-point number.
 */

double rint (double x) {
	double low = floor(x);
	if (fmod(x,low) >= (double)0.5)
		return(ceil(x));
	else
		return(low);
}

