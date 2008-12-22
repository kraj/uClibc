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
 * wrapper sinh(x)
 */

#include "math.h"
#include "math_private.h"

double sinh(double x)		/* wrapper sinh */
{
#ifdef _IEEE_LIBM
	return __ieee754_sinh(x);
#else
	double z;
	z = __ieee754_sinh(x);
	if(_LIB_VERSION == _IEEE_) return z;
	if(!isfinite(z)&&isfinite(x)) {
	    return __kernel_standard(x,x,25); /* sinh overflow */
	} else
	    return z;
#endif
}
libm_hidden_def(sinh)
