/* vi: set sw=4 ts=4: */
/*
 * Copyright (C) 2008 by Bernhard Reutner-Fischer <uclibc@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */
#include <math.h>
#include "math_private.h"

double log2 (double d)
{
	return __ieee754_log2 (d);
}
