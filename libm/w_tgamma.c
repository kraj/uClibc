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
 * Return  the logarithm of the Gamma function of x or the Gamma function of x,
 * depending on the library mode.
 */

#include "math.h"
#include "math_private.h"

double tgamma(double x)
{
        double y;
	int local_signgam;

	y = __ieee754_gamma_r(x, &local_signgam);
	if (local_signgam < 0)
		y = -y;
#ifndef _IEEE_LIBM
	if (_LIB_VERSION == _IEEE_)
		return y;
	if (!isfinite(y) && isfinite(x)) {
		if (floor(x) == x && x <= 0.0)
			return __kernel_standard(x, x, 41); /* tgamma pole */
		return __kernel_standard(x, x, 40); /* tgamma overflow */
	}
#endif
	return y;
}
libm_hidden_def(tgamma)
