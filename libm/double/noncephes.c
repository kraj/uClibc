/*
 * This file contains math functions missing from the Cephes library.
 *
 * May 22, 2001         Manuel Novoa III
 *
 *    Added modf and fmod.
 *
 * TODO:
 *    Break out functions into seperate object files as is done
 *       by (for example) stdio.  Also do this with cephes files.
 */

#include <math.h>
#include <errno.h>

#undef UNK

/* Set this to nonzero to enable a couple of shortcut tests in fmod. */
#define SPEED_OVER_SIZE 0

/**********************************************************************/

double modf(double x, double *iptr)
{
	double y;

#ifdef UNK
	mtherr( "modf", DOMAIN );
	*iptr = NAN;
	return NAN;
#endif

#ifdef NANS
	if( isnan(x) ) {
		*iptr = x;
		return x;
	}
#endif

#ifdef INFINITIES
	if(!isfinite(x)) {
		*iptr = x;				/* Matches glibc, but returning NAN */
		return 0;				/* makes more sense to me... */
	}
#endif

	if (x < 0) {				/* Round towards 0. */
		y = ceil(x);
	} else {
		y = floor(x);
	}

	*iptr = y;
	return x - y;
}

/**********************************************************************/

extern double NAN;

double fmod(double x, double y)
{
	double z;
	int negative, ex, ey;

#ifdef UNK
	mtherr( "fmod", DOMAIN );
	return NAN;
#endif

#ifdef NANS
	if( isnan(x) || isnan(y) ) {
		errno = EDOM;
		return NAN; 
	}
#endif

	if (y == 0) {
		errno = EDOM;
		return NAN; 
	}

#ifdef INFINITIES
	if(!isfinite(x)) {
		errno = EDOM;
		return NAN;
	}

#if SPEED_OVER_SIZE
	if(!isfinite(y)) {
		return x;
	}
#endif
#endif

#if SPEED_OVER_SIZE
	if (x == 0) {
		return 0;
	}
#endif

	negative = 0;
	if (x < 0) {
		negative = 1;
		x = -x;
	}

	if (y < 0) {
		y = -y;
	}

	frexp(y,&ey);
	while (x >= y) {
		frexp(x,&ex);
		z = ldexp(y,ex-ey);
		if (z > x) {
			z /= 2;
		}
		x -= z;
	}

	if (negative) {
		return -x;
	} else {
		return x;
	}
}
