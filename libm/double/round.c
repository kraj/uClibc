/*
 * June 19, 2001       Manuel Novoa III
 *
 * Replaced cephes round (which was actually round to nearest or even)
 * with a (really lame actually) version that always rounds away from 0
 * in conformance with ANSI/ISO.
 *
 * This doesn't check for inf or nan (hence the lame part) but the
 * cephes function it replaces didn't either.  I plan to deal with
 * those issues when I rework things w.r.t. common code.
 *
 * Also, for now rename the original cephes round routine to rint since
 * it behaves the same for the default rounding mode (round to nearest).
 * This will have to be changed off course when floating point env
 * control functions are added.
 */

#include <math.h>

double round(x)
double x;
{
	double ax, fax;

	ax = fabs(x);
	fax = floor(ax);
	if (ax - fax >= 0.5) {
		fax += 1.0;
	}
	if (x < 0) {
		x = -fax;
	} else {
		x = fax;
	}
	return x;
}

/***********************************************************************/
/*
 * Returns the nearest integer to x as a double precision
 * floating point result.  If x ends in 0.5 exactly, the
 * nearest even integer is chosen.
 * /
/*
Originally round from
Cephes Math Library Release 2.1:  January, 1989
Copyright 1984, 1987, 1989 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

double rint(x)
double x;
{
double y, r;

/* Largest integer <= x */
y = floor(x);

/* Fractional part */
r = x - y;

/* Round up to nearest. */
if( r > 0.5 )
	goto rndup;

/* Round to even */
if( r == 0.5 )
	{
	r = y - 2.0 * floor( 0.5 * y );
	if( r == 1.0 )
		{
rndup:
		y += 1.0;
		}
	}

/* Else round down. */
return(y);
}
