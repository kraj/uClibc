/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/*
 * __ieee754_scalb(x, fn) is provide for
 * passing various standard test suite. One
 * should use scalbn() instead.
 */

#include "math.h"
#include "math_private.h"
#include <errno.h>

#ifdef _SCALB_INT
double attribute_hidden __ieee754_scalb(double x, int fn)
#else
double attribute_hidden __ieee754_scalb(double x, double fn)
#endif
{
#ifdef _SCALB_INT
	return scalbn(x,fn);
//TODO: just alias it to scalbn?
#else
	if (isnan(x)||isnan(fn)) return x*fn;
	if (!isfinite(fn)) {
	    if(fn>0.0) return x*fn;
	    else       return x/(-fn);
	}
	if (rint(fn)!=fn) return (fn-fn)/(fn-fn);
	if ( fn > 65000.0) return scalbn(x, 65000);
	if (-fn > 65000.0) return scalbn(x,-65000);
	return scalbn(x,(int)fn);
#endif
}

/*
 * wrapper scalb(double x, double fn) is provide for
 * passing various standard test suite. One
 * should use scalbn() instead.
 */
#ifndef _IEEE_LIBM
# ifdef _SCALB_INT
double scalb(double x, int fn)
# else
double scalb(double x, double fn)
# endif
{
	double z = __ieee754_scalb(x, fn);
	if (_LIB_VERSION == _IEEE_)
		return z;
	if (!(isfinite(z) || isnan(z)) && isfinite(x))
		return __kernel_standard(x, (double)fn, 32); /* scalb overflow */
	if (z == 0.0 && z != x)
		return __kernel_standard(x, (double)fn, 33); /* scalb underflow */
# ifndef _SCALB_INT
	if (!isfinite(fn))
		errno = ERANGE;
# endif
	return z;
}
#else
strong_alias(__ieee754_scalb, scalb)
#endif
