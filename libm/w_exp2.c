
/* @(#)w_exp2.c 5.1 93/09/24 */
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

#ifdef __STDC__
	double exp2(double x)
#else
	double exp2(x)
	double x;
#endif
{
  return pow(2.0, x);
}
libm_hidden_def(exp2)
