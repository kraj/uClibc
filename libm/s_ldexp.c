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

#include "math.h"
#include "math_private.h"
#include <errno.h>

double ldexp(double value, int exp)
{
	if(!isfinite(value)||value==0.0) return value;
	value = scalbn(value,exp);
	if(!isfinite(value)||value==0.0) errno = ERANGE;
	return value;
}
libm_hidden_def(ldexp)
