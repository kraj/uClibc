/*
 * cabs() wrapper for hypot().
 *
 * Written by J.T. Conklin, <jtc@wimsey.com>
 * Placed into the Public Domain, 1994.
 */

#include <complex.h>
#include <math.h>


double cabs(double _Complex z)
{
	return hypot(__real__ z, __imag__ z);
}
libm_hidden_def(cabs)
