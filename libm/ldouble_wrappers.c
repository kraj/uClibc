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

#include <features.h>
/* Prevent math.h from defining colliding inlines */
#undef __USE_EXTERN_INLINES
#include "math.h"
#include <complex.h>

#define WRAPPER1(func) \
long double func##l(long double x) \
{ \
	return (long double) func((double) x); \
}
#define int_WRAPPER1(func) \
int func##l(long double x) \
{ \
	return func((double) x); \
}
#define long_WRAPPER1(func) \
long func##l(long double x) \
{ \
	return func((double) x); \
}
#define long_long_WRAPPER1(func) \
long long func##l(long double x) \
{ \
	return func((double) x); \
}

#if defined __i386__ && defined __OPTIMIZE__
# undef WRAPPER1
# undef int_WRAPPER1
# undef long_WRAPPER1
# undef long_long_WRAPPER1
/* gcc 4.3.1 generates really ugly code with redundant pair of store/load:
 *	sub	$0x10,%esp
 *	fldt	0x14(%esp)
 *	fstpl	0x8(%esp)
 *	fldl	0x8(%esp) <-- ??
 *	fstpl	(%esp)    <-- ??
 *	call	function
 *	add	$0x10,%esp
 *	ret
 * I can hope newer gcc will eliminate that. However, I don't think
 * it will be smart enough to reuse argument stack space and use
 * jump instead of call. Let's do it by hand.
 * The asm below loads long double x into st(0), then stores it back
 * to the same location, but as a double. At this point, stack looks
 * exactly as "double func(double)" expects it to be.
 * The return value is returned in st(0) per ABI in both cases (returning
 * a long double or returning a double). So we can simply jump to func.
 * Using __GI_func in jump to make optimized intra-library jump.
 * gcc will still generate a useless "ret" after asm. Oh well...
 */
# define WRAPPER1(func) \
long double func##l(long double x) \
{ \
	long double st_top; \
	__asm ( \
	"	fldt	%1\n" \
	"	fstpl	%1\n" \
	"	jmp	" __stringify(__GI_##func) "\n" \
	: "=t" (st_top) \
	: "m" (x) \
	); \
	return st_top; \
}
# define int_WRAPPER1(func) \
int func##l(long double x) \
{ \
	int ret; \
	__asm ( \
	"	fldt	%1\n" \
	"	fstpl	%1\n" \
	"	jmp	" __stringify(__GI_##func) "\n" \
	: "=a" (ret) \
	: "m" (x) \
	); \
	return ret; \
}
# define long_WRAPPER1(func) \
long func##l(long double x) \
{ \
	long ret; \
	__asm ( \
	"	fldt	%1\n" \
	"	fstpl	%1\n" \
	"	jmp	" __stringify(__GI_##func) "\n" \
	: "=a" (ret) \
	: "m" (x) \
	); \
	return ret; \
}
# define long_long_WRAPPER1(func) \
long long func##l(long double x) \
{ \
	long long ret; \
	__asm ( \
	"	fldt	%1\n" \
	"	fstpl	%1\n" \
	"	jmp	" __stringify(__GI_##func) "\n" \
	: "=A" (ret) \
	: "m" (x) \
	); \
	return ret; \
}
#endif /* __i386__ && __OPTIMIZE__ */

#if defined __NO_LONG_DOUBLE_MATH
# define int_WRAPPER_C99(func) /* not needed */
# else
# define int_WRAPPER_C99(func) \
int func##l(long double x) \
{ \
    return func((double) x); \
} \
libm_hidden_def(func##l)
#endif

/* Implement the following, as defined by SuSv3 */
#if 0
long double acoshl(long double);
long double acosl(long double);
long double asinhl(long double);
long double asinl(long double);
long double atan2l(long double, long double);
long double atanhl(long double);
long double atanl(long double);
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
WRAPPER1(acosh)
#endif

#ifdef L_acosl
WRAPPER1(acos)
#endif

#ifdef L_asinhl
WRAPPER1(asinh)
#endif

#ifdef L_asinl
WRAPPER1(asin)
#endif

#ifdef L_atan2l
long double atan2l (long double x, long double y)
{
	return (long double) atan2( (double)x, (double)y );
}
#endif

#ifdef L_atanhl
WRAPPER1(atanh)
#endif

#ifdef L_atanl
WRAPPER1(atan)
#endif

#ifdef L_cargl
long double cargl (long double complex x)
{
	return (long double) carg( (double complex)x );
}
#endif

#ifdef L_cbrtl
WRAPPER1(cbrt)
#endif

#ifdef L_ceill
WRAPPER1(ceil)
#endif

#ifdef L_copysignl
long double copysignl (long double x, long double y)
{
	return (long double) copysign( (double)x, (double)y );
}
#endif

#ifdef L_coshl
WRAPPER1(cosh)
#endif

#ifdef L_cosl
WRAPPER1(cos)
#endif

#ifdef L_erfcl
WRAPPER1(erfc)
#endif

#ifdef L_erfl
WRAPPER1(erf)
#endif

#ifdef L_exp2l
WRAPPER1(exp2)
#endif

#ifdef L_expl
WRAPPER1(exp)
#endif

#ifdef L_expm1l
WRAPPER1(expm1)
#endif

#ifdef L_fabsl
WRAPPER1(fabs)
#endif

#ifdef L_fdiml
long double fdiml (long double x, long double y)
{
	return (long double) fdim( (double)x, (double)y );
}
#endif

#ifdef L_floorl
WRAPPER1(floor)
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
long double frexpl (long double x, int *ex)
{
	return (long double) frexp( (double)x, ex );
}
#endif

#ifdef L_gammal
/* WRAPPER1(gamma) won't work, tries to call __GI_xxx,
 * and gamma() hasn't got one. */
long double gammal(long double x)
{
	return (long double) gamma((double) x);
}
#endif

#ifdef L_hypotl
long double hypotl (long double x, long double y)
{
	return (long double) hypot( (double)x, (double)y );
}
#endif

#ifdef L_ilogbl
int_WRAPPER1(ilogb)
#endif

#ifdef L_ldexpl
long double ldexpl (long double x, int ex)
{
	return (long double) ldexp( (double)x, ex );
}
#endif

#ifdef L_lgammal
WRAPPER1(lgamma)
#endif

#ifdef L_llrintl
long_long_WRAPPER1(llrint)
#endif

#ifdef L_llroundl
long_long_WRAPPER1(llround)
#endif

#ifdef L_log10l
WRAPPER1(log10)
#endif

#ifdef L_log1pl
WRAPPER1(log1p)
#endif

#ifdef L_log2l
/* WRAPPER1(log2) won't work */
long double log2l(long double x)
{
	return (long double) log2((double)x);
}
#endif

#ifdef L_logbl
WRAPPER1(logb)
#endif

#ifdef L_logl
WRAPPER1(log)
#endif

#ifdef L_lrintl
long_WRAPPER1(lrint)
#endif

#ifdef L_lroundl
long_WRAPPER1(lround)
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
WRAPPER1(nearbyint)
#endif

#ifdef L_nextafterl
long double nextafterl (long double x, long double y)
{
	return (long double) nextafter( (double)x, (double)y );
}
#endif

/* Disabled in Makefile.in */
#if 0 /* def L_nexttowardl */
long double nexttowardl (long double x, long double y)
{
	return (long double) nexttoward( (double)x, (double)y );
}
libm_hidden_def(nexttowardl)
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
WRAPPER1(rint)
#endif

#ifdef L_roundl
WRAPPER1(round)
#endif

#ifdef L_scalblnl
long double scalblnl (long double x, long ex)
{
	return (long double) scalbln( (double)x, ex );
}
#endif

#ifdef L_scalbnl
long double scalbnl (long double x, int ex)
{
	return (long double) scalbn( (double)x, ex );
}
#endif

/* scalb is an obsolete function */

#ifdef L_sinhl
WRAPPER1(sinh)
#endif

#ifdef L_sinl
WRAPPER1(sin)
#endif

#ifdef L_sqrtl
WRAPPER1(sqrt)
#endif

#ifdef L_tanhl
WRAPPER1(tanh)
#endif

#ifdef L_tanl
WRAPPER1(tan)
#endif

#ifdef L_tgammal
WRAPPER1(tgamma)
#endif

#ifdef L_truncl
WRAPPER1(trunc)
#endif

#ifdef L_significandl
/* WRAPPER1(significand) won't work, tries to call __GI_xxx,
 * and significand() hasn't got one. */
long double significandl(long double x)
{
	return (long double) significand((double) x);
}
#endif

#ifdef __DO_C99_MATH__

#ifdef L___fpclassifyl
int_WRAPPER_C99(__fpclassify)
#endif

#ifdef L___finitel
int_WRAPPER_C99(__finite)
#endif

#ifdef L___signbitl
int_WRAPPER_C99(__signbit)
#endif

#ifdef L___isnanl
int_WRAPPER_C99(__isnan)
#endif

#ifdef L___isinfl
int_WRAPPER_C99(__isinf)
#endif

#endif
