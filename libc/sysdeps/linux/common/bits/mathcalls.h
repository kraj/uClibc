/* Prototype declarations for math functions; helper file for <math.h>.
   Copyright (C) 1996-2002, 2003, 2006 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* NOTE: Because of the special way this file is used by <math.h>, this
   file must NOT be protected from multiple inclusion as header files
   usually are.

   This file provides prototype declarations for the math functions.
   Most functions are declared using the macro:

   __MATHCALL (NAME,[_r], (ARGS...));

   This means there is a function `NAME' returning `double' and a function
   `NAMEf' returning `float'.  Each place `_Mdouble_' appears in the
   prototype, that is actually `double' in the prototype for `NAME' and
   `float' in the prototype for `NAMEf'.  Reentrant variant functions are
   called `NAME_r' and `NAMEf_r'.

   Functions returning other types like `int' are declared using the macro:

   __MATHDECL (TYPE, NAME,[_r], (ARGS...));

   This is just like __MATHCALL but for a function returning `TYPE'
   instead of `_Mdouble_'.  In all of these cases, there is still
   both a `NAME' and a `NAMEf' that takes `float' arguments.

   Note that there must be no whitespace before the argument passed for
   NAME, to make token pasting work with -traditional.  */

#ifndef _MATH_H
# error "Never include <bits/mathcalls.h> directly; include <math.h> instead."
#endif


/* Trigonometric functions.  */

_Mdouble_BEGIN_NAMESPACE
/* Arc cosine of X.  */
__MATHCALL (acos,, (_Mdouble_ __x));
libm_hidden_proto(acos)
/* Arc sine of X.  */
__MATHCALL (asin,, (_Mdouble_ __x));
libm_hidden_proto(asin)
/* Arc tangent of X.  */
__MATHCALL (atan,, (_Mdouble_ __x));
libm_hidden_proto(atan)
/* Arc tangent of Y/X.  */
__MATHCALL (atan2,, (_Mdouble_ __y, _Mdouble_ __x));
libm_hidden_proto(atan2)

/* Cosine of X.  */
__MATHCALL (cos,, (_Mdouble_ __x));
libm_hidden_proto(cos)
/* Sine of X.  */
__MATHCALL (sin,, (_Mdouble_ __x));
libm_hidden_proto(sin)
/* Tangent of X.  */
__MATHCALL (tan,, (_Mdouble_ __x));
libm_hidden_proto(tan)

/* Hyperbolic functions.  */

/* Hyperbolic cosine of X.  */
__MATHCALL (cosh,, (_Mdouble_ __x));
libm_hidden_proto(cosh)
/* Hyperbolic sine of X.  */
__MATHCALL (sinh,, (_Mdouble_ __x));
libm_hidden_proto(sinh)
/* Hyperbolic tangent of X.  */
__MATHCALL (tanh,, (_Mdouble_ __x));
libm_hidden_proto(tanh)
_Mdouble_END_NAMESPACE

#if 0 /*def __USE_GNU*/
/* Cosine and sine of X.  */
__MATHDECL (void,sincos,,
	    (_Mdouble_ __x, _Mdouble_ *__sinx, _Mdouble_ *__cosx));
libm_hidden_proto(sincos)
#endif

#if defined __USE_MISC || defined __USE_XOPEN_EXTENDED || defined __USE_ISOC99
__BEGIN_NAMESPACE_C99
/* Hyperbolic arc cosine of X.  */
__MATHCALL (acosh,, (_Mdouble_ __x));
libm_hidden_proto(acosh)
/* Hyperbolic arc sine of X.  */
__MATHCALL (asinh,, (_Mdouble_ __x));
libm_hidden_proto(asinh)
/* Hyperbolic arc tangent of X.  */
__MATHCALL (atanh,, (_Mdouble_ __x));
libm_hidden_proto(atanh)
__END_NAMESPACE_C99
#endif

/* Exponential and logarithmic functions.  */

_Mdouble_BEGIN_NAMESPACE
/* Exponential function of X.  */
__MATHCALL (exp,, (_Mdouble_ __x));
libm_hidden_proto(exp)

/* Break VALUE into a normalized fraction and an integral power of 2.  */
__MATHCALL (frexp,, (_Mdouble_ __x, int *__exponent));
libm_hidden_proto(frexp)

/* X times (two to the EXP power).  */
__MATHCALL (ldexp,, (_Mdouble_ __x, int __exponent));
libm_hidden_proto(ldexp)

/* Natural logarithm of X.  */
__MATHCALL (log,, (_Mdouble_ __x));
libm_hidden_proto(log)

/* Base-ten logarithm of X.  */
__MATHCALL (log10,, (_Mdouble_ __x));
libm_hidden_proto(log10)

/* Break VALUE into integral and fractional parts.  */
__MATHCALL (modf,, (_Mdouble_ __x, _Mdouble_ *__iptr));
libm_hidden_proto(modf)
_Mdouble_END_NAMESPACE

#if 0 /*def __USE_GNU*/
/* A function missing in all standards: compute exponent to base ten.  */
__MATHCALL (exp10,, (_Mdouble_ __x));
libm_hidden_proto(exp10)
/* Another name occasionally used.  */
__MATHCALL (pow10,, (_Mdouble_ __x));
libm_hidden_proto(pow10)
#endif

#if defined __USE_MISC || defined __USE_XOPEN_EXTENDED || defined __USE_ISOC99
__BEGIN_NAMESPACE_C99
/* Return exp(X) - 1.  */
__MATHCALL (expm1,, (_Mdouble_ __x));
libm_hidden_proto(expm1)

/* Return log(1 + X).  */
__MATHCALL (log1p,, (_Mdouble_ __x));
libm_hidden_proto(log1p)

/* Return the base 2 signed integral exponent of X.  */
__MATHCALL (logb,, (_Mdouble_ __x));
libm_hidden_proto(logb)
__END_NAMESPACE_C99
#endif

#ifdef __USE_ISOC99
__BEGIN_NAMESPACE_C99
/* Compute base-2 exponential of X.  */
__MATHCALL (exp2,, (_Mdouble_ __x));
libm_hidden_proto(exp2)

/* Compute base-2 logarithm of X.  */
__MATHCALL (log2,, (_Mdouble_ __x));
libm_hidden_proto(log2)
__END_NAMESPACE_C99
#endif


/* Power functions.  */

_Mdouble_BEGIN_NAMESPACE
/* Return X to the Y power.  */
__MATHCALL (pow,, (_Mdouble_ __x, _Mdouble_ __y));
libm_hidden_proto(pow)

/* Return the square root of X.  */
__MATHCALL (sqrt,, (_Mdouble_ __x));
libm_hidden_proto(sqrt)
_Mdouble_END_NAMESPACE

#if defined __USE_MISC || defined __USE_XOPEN || defined __USE_ISOC99
__BEGIN_NAMESPACE_C99
/* Return `sqrt(X*X + Y*Y)'.  */
__MATHCALL (hypot,, (_Mdouble_ __x, _Mdouble_ __y));
libm_hidden_proto(hypot)
__END_NAMESPACE_C99
#endif

#if defined __USE_MISC || defined __USE_XOPEN_EXTENDED || defined __USE_ISOC99
__BEGIN_NAMESPACE_C99
/* Return the cube root of X.  */
__MATHCALL (cbrt,, (_Mdouble_ __x));
libm_hidden_proto(cbrt)
__END_NAMESPACE_C99
#endif


/* Nearest integer, absolute value, and remainder functions.  */

_Mdouble_BEGIN_NAMESPACE
/* Smallest integral value not less than X.  */
__MATHCALLX (ceil,, (_Mdouble_ __x), (__const__));
libm_hidden_proto(ceil)

/* Absolute value of X.  */
__MATHCALLX (fabs,, (_Mdouble_ __x), (__const__));
libm_hidden_proto(fabs)

/* Largest integer not greater than X.  */
__MATHCALLX (floor,, (_Mdouble_ __x), (__const__));
libm_hidden_proto(floor)

/* Floating-point modulo remainder of X/Y.  */
__MATHCALL (fmod,, (_Mdouble_ __x, _Mdouble_ __y));
libm_hidden_proto(fmod)


/* Return 0 if VALUE is finite or NaN, +1 if it
   is +Infinity, -1 if it is -Infinity.  */
__MATHDECL_1 (int,__isinf,, (_Mdouble_ __value)) __attribute__ ((__const__));
libm_hidden_proto(__isinf)

/* Return nonzero if VALUE is finite and not NaN.  */
__MATHDECL_1 (int,__finite,, (_Mdouble_ __value)) __attribute__ ((__const__));
libm_hidden_proto(__finite)
_Mdouble_END_NAMESPACE

#ifdef __USE_MISC
/* Return 0 if VALUE is finite or NaN, +1 if it
   is +Infinity, -1 if it is -Infinity.  */
__MATHDECL_1 (int,isinf,, (_Mdouble_ __value)) __attribute__ ((__const__));
libm_hidden_proto(isinf)

/* Return nonzero if VALUE is finite and not NaN.  */
__MATHDECL_1 (int,finite,, (_Mdouble_ __value)) __attribute__ ((__const__));
libm_hidden_proto(finite)

/* Return the remainder of X/Y.  */
__MATHCALL (drem,, (_Mdouble_ __x, _Mdouble_ __y));
libm_hidden_proto(drem)


/* Return the fractional part of X after dividing out `ilogb (X)'.  */
__MATHCALL (significand,, (_Mdouble_ __x));
libm_hidden_proto(significand)
#endif /* Use misc.  */

#if defined __USE_MISC || defined __USE_ISOC99
__BEGIN_NAMESPACE_C99
/* Return X with its signed changed to Y's.  */
__MATHCALLX (copysign,, (_Mdouble_ __x, _Mdouble_ __y), (__const__));
libm_hidden_proto(copysign)
__END_NAMESPACE_C99
#endif

#ifdef __USE_ISOC99
__BEGIN_NAMESPACE_C99
/* Return representation of NaN for double type.  */
__MATHCALLX (nan,, (__const char *__tagb), (__const__));
libm_hidden_proto(nan)
__END_NAMESPACE_C99
#endif


/* Return nonzero if VALUE is not a number.  */
__MATHDECL_1 (int,__isnan,, (_Mdouble_ __value)) __attribute__ ((__const__));
libm_hidden_proto(__isnan)

#if defined __USE_MISC || defined __USE_XOPEN
/* Return nonzero if VALUE is not a number.  */
__MATHDECL_1 (int,isnan,, (_Mdouble_ __value)) __attribute__ ((__const__));
libm_hidden_proto(isnan)

/* Bessel functions.  */
__MATHCALL (j0,, (_Mdouble_));
libm_hidden_proto(j0)
__MATHCALL (j1,, (_Mdouble_));
libm_hidden_proto(j1)
__MATHCALL (jn,, (int, _Mdouble_));
libm_hidden_proto(jn)
__MATHCALL (y0,, (_Mdouble_));
libm_hidden_proto(y0)
__MATHCALL (y1,, (_Mdouble_));
libm_hidden_proto(y1)
__MATHCALL (yn,, (int, _Mdouble_));
libm_hidden_proto(yn)
#endif


#if defined __USE_MISC || defined __USE_XOPEN || defined __USE_ISOC99
__BEGIN_NAMESPACE_C99
/* Error and gamma functions.  */
__MATHCALL (erf,, (_Mdouble_));
libm_hidden_proto(erf)
__MATHCALL (erfc,, (_Mdouble_));
libm_hidden_proto(erfc)
__MATHCALL (lgamma,, (_Mdouble_));
libm_hidden_proto(lgamma)
__END_NAMESPACE_C99
#endif

#ifdef __USE_ISOC99
__BEGIN_NAMESPACE_C99
/* True gamma function.  */
__MATHCALL (tgamma,, (_Mdouble_));
libm_hidden_proto(tgamma)
__END_NAMESPACE_C99
#endif

#if defined __USE_MISC || defined __USE_XOPEN
/* Obsolete alias for `lgamma'.  */
__MATHCALL (gamma,, (_Mdouble_));
libm_hidden_proto(gamma)
#endif

#ifdef __USE_MISC
/* Reentrant version of lgamma.  This function uses the global variable
   `signgam'.  The reentrant version instead takes a pointer and stores
   the value through it.  */
__MATHCALL (lgamma,_r, (_Mdouble_, int *__signgamp));
libm_hidden_proto(lgamma_r)
#endif


#if defined __USE_MISC || defined __USE_XOPEN_EXTENDED || defined __USE_ISOC99
__BEGIN_NAMESPACE_C99
/* Return the integer nearest X in the direction of the
   prevailing rounding mode.  */
__MATHCALL (rint,, (_Mdouble_ __x));
libm_hidden_proto(rint)

/* Return X + epsilon if X < Y, X - epsilon if X > Y.  */
__MATHCALLX (nextafter,, (_Mdouble_ __x, _Mdouble_ __y), (__const__));
libm_hidden_proto(nextafter)
# if defined __USE_ISOC99 && !defined __LDBL_COMPAT
__MATHCALLX (nexttoward,, (_Mdouble_ __x, long double __y), (__const__));
libm_hidden_proto(nexttoward)
# endif

/* Return the remainder of integer divison X / Y with infinite precision.  */
__MATHCALL (remainder,, (_Mdouble_ __x, _Mdouble_ __y));
libm_hidden_proto(remainder)

# if defined __USE_MISC || defined __USE_ISOC99
/* Return X times (2 to the Nth power).  */
__MATHCALL (scalbn,, (_Mdouble_ __x, int __n));
libm_hidden_proto(scalbn)
# endif

/* Return the binary exponent of X, which must be nonzero.  */
__MATHDECL (int,ilogb,, (_Mdouble_ __x));
libm_hidden_proto(ilogb)
#endif

#ifdef __USE_ISOC99
/* Return X times (2 to the Nth power).  */
__MATHCALL (scalbln,, (_Mdouble_ __x, long int __n));
libm_hidden_proto(scalbln)

/* Round X to integral value in floating-point format using current
   rounding direction, but do not raise inexact exception.  */
__MATHCALL (nearbyint,, (_Mdouble_ __x));
libm_hidden_proto(nearbyint)

/* Round X to nearest integral value, rounding halfway cases away from
   zero.  */
__MATHCALLX (round,, (_Mdouble_ __x), (__const__));
libm_hidden_proto(round)

/* Round X to the integral value in floating-point format nearest but
   not larger in magnitude.  */
__MATHCALLX (trunc,, (_Mdouble_ __x), (__const__));
libm_hidden_proto(trunc)

/* Compute remainder of X and Y and put in *QUO a value with sign of x/y
   and magnitude congruent `mod 2^n' to the magnitude of the integral
   quotient x/y, with n >= 3.  */
__MATHCALL (remquo,, (_Mdouble_ __x, _Mdouble_ __y, int *__quo));
libm_hidden_proto(remquo)


/* Conversion functions.  */

/* Round X to nearest integral value according to current rounding
   direction.  */
__MATHDECL (long int,lrint,, (_Mdouble_ __x));
libm_hidden_proto(lrint)
__MATHDECL (long long int,llrint,, (_Mdouble_ __x));
libm_hidden_proto(llrint)

/* Round X to nearest integral value, rounding halfway cases away from
   zero.  */
__MATHDECL (long int,lround,, (_Mdouble_ __x));
libm_hidden_proto(lround)
__MATHDECL (long long int,llround,, (_Mdouble_ __x));
libm_hidden_proto(llround)


/* Return positive difference between X and Y.  */
__MATHCALL (fdim,, (_Mdouble_ __x, _Mdouble_ __y));
libm_hidden_proto(fdim)

/* Return maximum numeric value from X and Y.  */
__MATHCALL (fmax,, (_Mdouble_ __x, _Mdouble_ __y));
libm_hidden_proto(fmax)

/* Return minimum numeric value from X and Y.  */
__MATHCALL (fmin,, (_Mdouble_ __x, _Mdouble_ __y));
libm_hidden_proto(fmin)


/* Classify given number.  */
__MATHDECL_1 (int, __fpclassify,, (_Mdouble_ __value))
     __attribute__ ((__const__));
libm_hidden_proto(__fpclassify)

/* Test for negative number.  */
__MATHDECL_1 (int, __signbit,, (_Mdouble_ __value))
     __attribute__ ((__const__));
libm_hidden_proto(__signbit)


/* Multiply-add function computed as a ternary operation.  */
__MATHCALL (fma,, (_Mdouble_ __x, _Mdouble_ __y, _Mdouble_ __z));
libm_hidden_proto(fma)
#endif /* Use ISO C99.  */

#if defined __USE_MISC || defined __USE_XOPEN_EXTENDED || defined __USE_ISOC99
__END_NAMESPACE_C99
#endif

#if defined __USE_MISC || defined __USE_XOPEN_EXTENDED
/* Return X times (2 to the Nth power).  */
__MATHCALL (scalb,, (_Mdouble_ __x, _Mdouble_ __n));
libm_hidden_proto(scalb)
#endif
