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

/* double gamma(double x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call gamma_r
 */

#include <math.h>
#include "math_private.h"

double gamma(double x)
{
#ifdef _IEEE_LIBM
	return __ieee754_lgamma_r(x, &signgam);
#else
	double y = __ieee754_lgamma_r(x, &signgam);
	if (_LIB_VERSION == _IEEE_)
		return y;
	if (!isfinite(y) && isfinite(x)) {
		if (floor(x) == x && x <= 0.0)
			return __kernel_standard(x, x, 41); /* gamma pole */
		return __kernel_standard(x, x, 40); /* gamma overflow */
	}
	return y;
#endif
}
