/* vi: set sw=4 ts=4: */
/*
 * Wrapper functions implementing all the long double math functions
 * defined by SuSv3 by actually calling the double version of
 * each function and then casting the result back to a long double
 * to return to the user.
 *
 * Copyright (C) 2005 by Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "math.h"
#include <complex.h>


/* Implement the following, as defined by SuSv3 */
#if 0
long double acoshl(long double);
long double acosl(long double);
long double asinhl(long double);
long double asinl(long double);
long double atan2l(long double, long double);
long double atanhl(long double);
long double atanl(long double);
long double cabsl(long double complex);
long double cargl(long double complex);
long double cbrtl(long double);
long double ceill(long double);
long double copysignl(long double, long double);
long double coshl(long double);
long double cosl(long double);
long double erfcl(long double);
long double erfl(long double);
long double exp2l(long double);
long double expl(long double);
long double expm1l(long double);
long double fabsl(long double);
long double fdiml(long double, long double);
long double floorl(long double);
long double fmal(long double, long double, long double);
long double fmaxl(long double, long double);
long double fminl(long double, long double);
long double fmodl(long double, long double);
long double frexpl(long double value, int *);
long double hypotl(long double, long double);
int         ilogbl(long double);
long double ldexpl(long double, int);
long double lgammal(long double);
long long   llrintl(long double);
long long   llroundl(long double);
long double log10l(long double);
long double log1pl(long double);
long double log2l(long double);
long double logbl(long double);
long double logl(long double);
long        lrintl(long double);
long        lroundl(long double);
long double modfl(long double, long double *);
long double nearbyintl(long double);
long double nextafterl(long double, long double);
long double nexttowardl(long double, long double);
long double powl(long double, long double);
long double remainderl(long double, long double);
long double remquol(long double, long double, int *);
long double rintl(long double);
long double roundl(long double);
long double scalblnl(long double, long);
long double scalbnl(long double, int);
long double sinhl(long double);
long double sinl(long double);
long double sqrtl(long double);
long double tanhl(long double);
long double tanl(long double);
long double tgammal(long double);
long double truncl(long double);
#endif

#ifdef L_acoshl
long double acoshl (long double x)
{
	return (long double) acosh( (double)x );
}
#endif


#ifdef L_acosl
long double acosl (long double x)
{
	return (long double) acos( (double)x );
}
#endif


#ifdef L_asinhl
long double asinhl (long double x)
{
	return (long double) asinh( (double)x );
}
#endif


#ifdef L_asinl
long double asinl (long double x)
{
	return (long double) asin( (double)x );
}
#endif


#ifdef L_atan2l
long double atan2l (long double x, long double y)
{
	return (long double) atan2( (double)x, (double)y );
}
#endif


#ifdef L_atanhl
long double atanhl (long double x)
{
	return (long double) atanh( (double)x );
}
#endif


#ifdef L_atanl
long double atanl (long double x)
{
	return (long double) atan( (double)x );
}
#endif


#ifdef L_cabsl
long double cabsl (long double complex x)
{
	return (long double) cabs( (double complex)x );
}
#endif


#ifdef L_cargl
long double cargl (long double complex x)
{
	return (long double) carg( (double complex)x );
}
#endif


#ifdef L_cbrtl
long double cbrtl (long double x)
{
	return (long double) cbrt( (double)x );
}
#endif


#ifdef L_ceill
long double ceill (long double x)
{
	return (long double) ceil( (double)x );
}
#endif


#ifdef L_copysignl
long double copysignl (long double x, long double y)
{
	return (long double) copysign( (double)x, (double)y );
}
#endif


#ifdef L_coshl
long double coshl (long double x)
{
	return (long double) cosh( (double)x );
}
#endif


#ifdef L_cosl
long double cosl (long double x)
{
	return (long double) cos( (double)x );
}
#endif


#ifdef L_erfcl
long double erfcl (long double x)
{
	return (long double) erfc( (double)x );
}
#endif


#ifdef L_erfl
long double erfl (long double x)
{
	return (long double) erf( (double)x );
}
#endif


#ifdef L_exp2l
long double exp2l (long double x)
{
	return (long double) exp2( (double)x );
}
#endif


#ifdef L_expl
long double expl (long double x)
{
	return (long double) exp( (double)x );
}
#endif


#ifdef L_expm1l
long double expm1l (long double x)
{
	return (long double) expm1( (double)x );
}
#endif


#ifdef L_fabsl
long double fabsl (long double x)
{
	return (long double) fabs( (double)x );
}
#endif


#ifdef L_fdiml
long double fdiml (long double x, long double y)
{
	return (long double) fdim( (double)x, (double)y );
}
#endif


#ifdef L_floorl
long double floorl (long double x)
{
	return (long double) floor( (double)x );
}
#endif


#ifdef L_fmal
long double fmal (long double x, long double y, long double z)
{
	return (long double) fma( (double)x, (double)y, (double)z );
}
#endif


#ifdef L_fmaxl
long double fmaxl (long double x, long double y)
{
	return (long double) fmax( (double)x, (double)y );
}
#endif


#ifdef L_fminl
long double fminl (long double x, long double y)
{
	return (long double) fmin( (double)x, (double)y );
}
#endif


#ifdef L_fmodl
long double fmodl (long double x, long double y)
{
	return (long double) fmod( (double)x, (double)y );
}
#endif


#ifdef L_frexpl
long double frexpl (long double x, int *exponent)
{
	return (long double) frexp( (double)x, exponent );
}
#endif


#ifdef L_hypotl
long double hypotl (long double x, long double y)
{
	return (long double) hypot( (double)x, (double)y );
}
#endif


#ifdef L_ilogbl
int ilogbl (long double x)
{
	return (long double) ilogb( (double)x );
}
#endif


#ifdef L_ldexpl
long double ldexpl (long double x, int exponent)
{
	return (long double) ldexp( (double)x, exponent );
}
#endif


#ifdef L_lgammal
long double lgammal (long double x)
{
	return (long double) lgamma( (double)x );
}
#endif


#ifdef L_llrintl
long long llrintl (long double x)
{
	return (long double) llrint( (double)x );
}
#endif


#ifdef L_llroundl
long long llroundl (long double x)
{
	return (long double) llround( (double)x );
}
#endif

#ifdef L_log10l
long double log10l (long double x)
{
	return (long double) log10( (double)x );
}
#endif


#ifdef L_log1pl
long double log1pl (long double x)
{
	return (long double) log1p( (double)x );
}
#endif


#ifdef L_log2l
long double log2l (long double x)
{
	return (long double) log2( (double)x );
}
#endif


#ifdef L_logbl
long double logbl (long double x)
{
	return (long double) logb( (double)x );
}
#endif


#ifdef L_logl
long double logl (long double x)
{
	return (long double) log( (double)x );
}
#endif


#ifdef L_lrintl
long lrintl (long double x)
{
	return (long double) lrint( (double)x );
}
#endif


#ifdef L_lroundl
long lroundl (long double x)
{
	return (long double) lround( (double)x );
}
#endif


#ifdef L_modfl
long double modfl (long double x, long double *iptr)
{
	double y, result;
	result = modf ( x, &y );
	*iptr = (long double)y;
	return (long double) result;

}
#endif


#ifdef L_nearbyintl
long double nearbyintl (long double x)
{
	return (long double) nearbyint( (double)x );
}
#endif


#ifdef L_nextafterl
long double nextafterl (long double x, long double y)
{
	return (long double) nextafter( (double)x, (double)y );
}
#endif


#ifdef L_nexttowardl
long double nexttowardl (long double x, long double y)
{
	return (long double) nexttoward( (double)x, (double)y );
}
#endif


#ifdef L_powl
long double powl (long double x, long double y)
{
	return (long double) pow( (double)x, (double)y );
}
#endif


#ifdef L_remainderl
long double remainderl (long double x, long double y)
{
	return (long double) remainder( (double)x, (double)y );
}
#endif


#ifdef L_remquol
long double remquol (long double x, long double y, int *quo)
{
	return (long double) remquo( (double)x, (double)y, quo );
}
#endif


#ifdef L_rintl
long double rintl (long double x)
{
	return (long double) rint( (double)x );
}
#endif


#ifdef L_roundl
long double roundl (long double x)
{
	return (long double) round( (double)x );
}
#endif


#ifdef L_scalblnl
long double scalblnl (long double x, long exponent)
{
	return (long double) scalbln( (double)x, exponent );
}
#endif


#ifdef L_scalbnl
long double scalbnl (long double x, int exponent)
{
	return (long double) scalbn( (double)x, exponent );
}
#endif


#ifdef L_sinhl
long double sinhl (long double x)
{
	return (long double) sinh( (double)x );
}
#endif


#ifdef L_sinl
long double sinl (long double x)
{
	return (long double) sin( (double)x );
}
#endif


#ifdef L_sqrtl
long double sqrtl (long double x)
{
	return (long double) sqrt( (double)x );
}
#endif


#ifdef L_tanhl
long double tanhl (long double x)
{
	return (long double) tanh( (double)x );
}
#endif


#ifdef L_tanl
long double tanl (long double x)
{
	return (long double) tan( (double)x );
}
#endif


#ifdef L_tgammal
long double tgammal (long double x)
{
	return (long double) tgamma( (double)x );
}
#endif


#ifdef L_truncl
long double truncl (long double x)
{
	return (long double) trunc( (double)x );
}
#endif


#ifdef __DO_C99_MATH__

#ifdef L_fpclassifyl
int __fpclassifyl (long double x)
{
	return __fpclassify ( (double) x );
}
libm_hidden_def(__fpclassifyl)
#endif

#ifdef L_finitel
int __finitel (long double x)
{
	return __finite ( (double)x );
}
libm_hidden_def(__finitel)
#endif

#ifdef L_signbitl
int __signbitl (long double x)
{
	return __signbitl ( (double)x );
}
libm_hidden_def(__signbitl)
#endif

#ifdef L_isnanl
int __isnanl (long double x)
{
	return __isnan ( (double)x );
}
libm_hidden_def(__isnanl)
#endif

#ifdef L_isinfl
int __isinfl (long double x)
{
	return __isinf ( (double)x );
}
libm_hidden_def(__isinfl)
#endif

#endif
