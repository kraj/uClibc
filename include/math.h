/*							mconf.h
 * <math.h>
 * ISO/IEC 9899:1999 -- Programming Languages C: 7.12 Mathematics 
 * Derived from the Cephes Math Library Release 2.3
 * Copyright 1984, 1987, 1989, 1995 by Stephen L. Moshier
 *
 *
 * DESCRIPTION:
 *
 * The file also includes a conditional assembly definition
 * for the type of computer arithmetic (IEEE, DEC, Motorola
 * IEEE, or UNKnown).
 * 
 * For Digital Equipment PDP-11 and VAX computers, certain
 * IBM systems, and others that use numbers with a 56-bit
 * significand, the symbol DEC should be defined.  In this
 * mode, most floating point constants are given as arrays
 * of octal integers to eliminate decimal to binary conversion
 * errors that might be introduced by the compiler.
 *
 * For little-endian computers, such as IBM PC, that follow the
 * IEEE Standard for Binary Floating Point Arithmetic (ANSI/IEEE
 * Std 754-1985), the symbol IBMPC should be defined.  These
 * numbers have 53-bit significands.  In this mode, constants
 * are provided as arrays of hexadecimal 16 bit integers.
 *
 * Big-endian IEEE format is denoted MIEEE.  On some RISC
 * systems such as Sun SPARC, double precision constants
 * must be stored on 8-byte address boundaries.  Since integer
 * arrays may be aligned differently, the MIEEE configuration
 * may fail on such machines.
 *
 * To accommodate other types of computer arithmetic, all
 * constants are also provided in a normal decimal radix
 * which one can hope are correctly converted to a suitable
 * format by the available C language compiler.  To invoke
 * this mode, define the symbol UNK.
 *
 * An important difference among these modes is a predefined
 * set of machine arithmetic constants for each.  The numbers
 * MACHEP (the machine roundoff error), MAXNUM (largest number
 * represented), and several other parameters are preset by
 * the configuration symbol.  Check the file const.c to
 * ensure that these values are correct for your computer.
 *
 * Configurations NANS, INFINITIES, MINUSZERO, and DENORMAL
 * may fail on many systems.  Verify that they are supposed
 * to work on your computer.
 */


#ifndef	_MATH_H
#define	_MATH_H	1

#include <features.h>

/* Type of computer arithmetic */

/* PDP-11, Pro350, VAX:
 */
/* #define DEC 1 */

/* Intel IEEE, low order words come first:
 */
/* #define IBMPC 1 */

/* Motorola IEEE, high order words come first
 * (Sun 680x0 workstation):
 */
/* #define MIEEE 1 */

/* UNKnown arithmetic, invokes coefficients given in
 * normal decimal format.  Beware of range boundary
 * problems (MACHEP, MAXLOG, etc. in const.c) and
 * roundoff problems in pow.c:
 * (Sun SPARCstation)
 */
#define UNK 1


/* Define if the `long double' type works.  */
#define HAVE_LONG_DOUBLE 1

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* Define if floating point words are bigendian.  */
/* #undef FLOAT_WORDS_BIGENDIAN */

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1


/* Define this `volatile' if your compiler thinks
 * that floating point arithmetic obeys the associative
 * and distributive laws.  It will defeat some optimizations
 * (but probably not enough of them).
 *
 * #define VOLATILE volatile
 */
#define VOLATILE

/* For 12-byte long doubles on an i386, pad a 16-bit short 0
 * to the end of real constants initialized by integer arrays.
 *
 * #define XPD 0,
 *
 * Otherwise, the type is 10 bytes long and XPD should be
 * defined blank (e.g., Microsoft C).
 *
 * #define XPD
 */
#define XPD 0,

/* Define to support tiny denormal numbers, else undefine. */
#define DENORMAL 1

/* Define to ask for infinity support, else undefine. */
#define INFINITIES 1

/* Define to ask for support of numbers that are Not-a-Number,
   else undefine.  This may automatically define INFINITIES in some files. */
#define NANS 1

/* Define to distinguish between -0.0 and +0.0.  */
#define MINUSZERO 1

/* Define 1 for ANSI C atan2() function
   and ANSI prototypes for float arguments.
   See atan.c and clog.c. */
#define ANSIC 1
#define ANSIPROT 1


/* Constant definitions for math error conditions */

#define DOMAIN		1	/* argument domain error */
#define SING		2	/* argument singularity */
#define OVERFLOW	3	/* overflow range error */
#define UNDERFLOW	4	/* underflow range error */
#define TLOSS		5	/* total loss of precision */
#define PLOSS		6	/* partial loss of precision */

#define EDOM		33
#define ERANGE		34

/* Complex numeral.  */
#ifdef __UCLIBC_HAS_DOUBLE__
typedef struct
	{
	double r;
	double i;
	} cmplx;
#endif

#ifdef __UCLIBC_HAS_FLOATS__
typedef struct
	{
	float r;
	float i;
	} cmplxf;
#endif

#ifdef __UCLIBC_HAS_LONG_DOUBLE__
/* Long double complex numeral.  */
typedef struct
	{
	long double r;
	long double i;
	} cmplxl;
#endif



/* Variable for error reporting.  See mtherr.c.  */
extern int mtherr(char *name, int code);
extern int merror;


/* If you define UNK, then be sure to set BIGENDIAN properly. */
#include <endian.h>
#if __BYTE_ORDER == __BIG_ENDIAN
#  define BIGENDIAN 1
#else /* __BYTE_ORDER == __LITTLE_ENDIAN */
#  define BIGENDIAN 0
#endif


#define __USE_ISOC9X
/* Get general and ISO C 9X specific information.  */
#include <bits/mathdef.h>
#undef INFINITY
#undef DECIMAL_DIG
#undef FP_ILOGB0
#undef FP_ILOGBNAN

/* Get the architecture specific values describing the floating-point
   evaluation.  The following symbols will get defined:

    float_t	floating-point type at least as wide as `float' used
		to evaluate `float' expressions
    double_t	floating-point type at least as wide as `double' used
		to evaluate `double' expressions

    FLT_EVAL_METHOD
		Defined to
		  0	if `float_t' is `float' and `double_t' is `double'
		  1	if `float_t' and `double_t' are `double'
		  2	if `float_t' and `double_t' are `long double'
		  else	`float_t' and `double_t' are unspecified

    INFINITY	representation of the infinity value of type `float'

    FP_FAST_FMA
    FP_FAST_FMAF
    FP_FAST_FMAL
		If defined it indicates that the `fma' function
		generally executes about as fast as a multiply and an add.
		This macro is defined only iff the `fma' function is
		implemented directly with a hardware multiply-add instructions.

    FP_ILOGB0	Expands to a value returned by `ilogb (0.0)'.
    FP_ILOGBNAN	Expands to a value returned by `ilogb (NAN)'.

    DECIMAL_DIG	Number of decimal digits supported by conversion between
		decimal and all internal floating-point formats.

*/

/* All floating-point numbers can be put in one of these categories.  */
enum
  {
    FP_NAN,
# define FP_NAN FP_NAN
    FP_INFINITE,
# define FP_INFINITE FP_INFINITE
    FP_ZERO,
# define FP_ZERO FP_ZERO
    FP_SUBNORMAL,
# define FP_SUBNORMAL FP_SUBNORMAL
    FP_NORMAL
# define FP_NORMAL FP_NORMAL
  };

/* Return number of classification appropriate for X.  */
#ifdef __UCLIBC_HAS_DOUBLE__
#  define fpclassify(x) \
     (sizeof (x) == sizeof (float) ?					      \
        __fpclassifyf (x)						      \
      : sizeof (x) == sizeof (double) ?					      \
        __fpclassify (x) : __fpclassifyl (x))
#else
#  define fpclassify(x) \
     (sizeof (x) == sizeof (float) ? __fpclassifyf (x) : __fpclassify (x))
#endif


#ifdef __UCLIBC_HAS_DOUBLE__
/* Return nonzero value if sign of X is negative.  */
extern int signbit(double x);
/* Return nonzero value if X is not +-Inf or NaN.  */
extern int isfinite(double x);
/* Return nonzero value if X is neither zero, subnormal, Inf, nor NaN.  */
# define isnormal(x) (fpclassify (x) == FP_NORMAL)
/* Return nonzero value if X is a NaN */
extern int isnan(double x);
#define isinf(x) \
     (sizeof (x) == sizeof (float) ?					      \
        __isinff (x)							      \
      : sizeof (x) == sizeof (double) ?					      \
        __isinf (x) : __isinfl (x))
# else
#  define isinf(x) \
     (sizeof (x) == sizeof (float) ? __isinff (x) : __isinf (x))
# endif


# ifdef __UCLIBC_HAS_LONG_DOUBLE__
/* Return nonzero value if sign of X is negative.  */
extern int signbitl(long double x);
/* Return nonzero value if X is not +-Inf or NaN.  */
extern int isfinitel(long double x);
/* Return nonzero value if X is a NaN */
extern int isnanl(long double x);
#endif


/* Some useful constants.  */
#if defined __USE_BSD || defined __USE_XOPEN
# define M_E		2.7182818284590452354	/* e */
# define M_LOG2E	1.4426950408889634074	/* log_2 e */
# define M_LOG10E	0.43429448190325182765	/* log_10 e */
# define M_LN2		0.69314718055994530942	/* log_e 2 */
# define M_LN10		2.30258509299404568402	/* log_e 10 */
# define M_PI		3.14159265358979323846	/* pi */
# define M_PI_2		1.57079632679489661923	/* pi/2 */
# define M_PI_4		0.78539816339744830962	/* pi/4 */
# define M_1_PI		0.31830988618379067154	/* 1/pi */
# define M_2_PI		0.63661977236758134308	/* 2/pi */
# define M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
# define M_SQRT2	1.41421356237309504880	/* sqrt(2) */
# define M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */
#endif
#ifdef __USE_GNU
# define M_El		M_E
# define M_LOG2El	M_LOG2E
# define M_LOG10El	M_LOG10E
# define M_LN2l		M_LN2
# define M_LN10l	M_LN10
# define M_PIl		M_PI
# define M_PI_2l	M_PI_2
# define M_PI_4l	M_PI_4
# define M_1_PIl	M_1_PI
# define M_2_PIl	M_2_PI
# define M_2_SQRTPIl	M_2_SQRTPI
# define M_SQRT2l	M_SQRT2
# define M_SQRT1_2l	M_SQRT1_2
#endif



#ifdef __UCLIBC_HAS_DOUBLE__
/* 7.12.4 Trigonometric functions */
extern double acos(double x);
extern double asin(double x);
extern double atan(double x);
extern double atan2(double y, double x);
extern double cos(double x);
extern double sin(double x);
extern double tan(double x);

/* 7.12.5 Hyperbolic functions */
extern double acosh(double x);
extern double asinh(double x);
extern double atanh(double x);
extern double cosh(double x);
extern double sinh(double x);
extern double tanh(double x);

/* 7.12.6 Exponential and logarithmic functions */
extern double exp(double x);
extern double exp2(double x);
extern double expm1(double x);
extern double frexp(double value, int *exp);
extern int ilogb(double x);
extern double ldexp(double x, int exp);
extern double log(double x);
extern double log10(double x);
extern double log1p(double x);
extern double log2(double x);
extern double logb(double x);
extern double modf(double value, double *iptr);
extern double scalbn(double x, int n);
extern double scalbln(double x, long int n);

/* 7.12.7 Power and absolute-value functions */
extern double fabs(double x);
extern double hypot(double x, double y);
extern double pow(double x, double y);
extern double sqrt(double x);

/* 7.12.8 Error and gamma functions */
extern double erf(double x);
extern double erfc(double x);
extern double lgamma(double x);
extern double tgamma(double x);

/* 7.12.9 Nearest integer functions */
extern double ceil(double x);
extern double floor(double x);
extern double nearbyint(double x);
extern double rint(double x);
extern long int lrint(double x);
extern long long int llrint(double x);
extern double round(double x);
extern long int lround(double x);
extern long long int llround(double x);
extern double trunc(double x);

/* 7.12.10 Remainder functions */
extern double fmod(double x, double y);
extern double remainder(double x, double y);
extern double remquo(double x, double y, int *quo);

/* 7.12.11 Manipulation functions */
extern double copysign(double x, double y);
extern double nan(const char *tagp);
extern double nextafter(double x, double y);

/* 7.12.12 Maximum, minimum, and positive difference functions */
extern double fdim(double x, double y);
extern double fmax(double x, double y);
extern double fmin(double x, double y);

/* 7.12.13 Floating multiply-add */
extern double fma(double x, double y, double z);
#endif	

#ifdef __UCLIBC_HAS_FLOATS__
/* 7.12.4 Trigonometric functions */
extern float acosf(float x);
extern float asinf(float x);
extern float atanf(float x);
extern float atan2f(float y, float x);
extern float cosf(float x);
extern float sinf(float x);
extern float tanf(float x);

/* 7.12.5 Hyperbolic functions */
extern float acoshf(float x);
extern float asinhf(float x);
extern float atanhf(float x);
extern float coshf(float x);
extern float sinhf(float x);
extern float tanhf(float x);

/* 7.12.6 Exponential and logarithmic functions */
extern float expf(float x);
extern float exp2f(float x);
extern float expm1f(float x);
extern float frexpf(float value, int *exp);
extern int ilogbf(float x);
extern float ldexpf(float x, int exp);
extern float logf(float x);
extern float log10f(float x);
extern float log1pf(float x);
extern float log2f(float x);
extern float logbf(float x);
extern float modff(float value, float *iptr);
extern float scalbnf(float x, int n);
extern float scalblnf(float x, long int n);

/* 7.12.7 Power and absolute-value functions */
extern float fabsf(float x);
extern float hypotf(float x, float y);
extern float powf(float x, float y);
extern float sqrtf(float x);

/* 7.12.8 Error and gamma functions */
extern float erff(float x);
extern float erfcf(float x);
extern float lgammaf(float x);
extern float tgammaf(float x);

/* 7.12.9 Nearest integer functions */
extern float ceilf(float x);
extern float floorf(float x);
extern float nearbyintf(float x);
extern float rintf(float x);
extern long int lrintf(float x);
extern long long int llrintf(float x);
extern float roundf(float x);
extern long int lroundf(float x);
extern long long int llroundf(float x);
extern float truncf(float x);

/* 7.12.10 Remainder functions */
extern float fmodf(float x, float y);
extern float remainderf(float x, float y);
extern float remquof(float x, float y, int *quo);

/* 7.12.11 Manipulation functions */
extern float copysignf(float x, float y);
extern float nanf(const char *tagp);
extern float nextafterf(float x, float y);

/* 7.12.12 Maximum, minimum, and positive difference functions */
extern float fdimf(float x, float y);
extern float fmaxf(float x, float y);
extern float fminf(float x, float y);

/* 7.12.13 Floating multiply-add */
extern float fmaf(float x, float y, float z);
#endif	

#ifdef __UCLIBC_HAS_LONG_DOUBLE__
/* 7.12.4 Trigonometric functions */
extern long double acosl(long double x);
extern long double asinl(long double x);
extern long double atanl(long double x);
extern long double atan2l(long double y, long double x);
extern long double cosl(long double x);
extern long double sinl(long double x);
extern long double tanl(long double x);

/* 7.12.5 Hyperbolic functions */
extern long double acoshl(long double x);
extern long double asinhl(long double x);
extern long double atanhl(long double x);
extern long double coshl(long double x);
extern long double sinhl(long double x);
extern long double tanhl(long double x);

/* 7.12.6 Exponential and logarithmic functions */
extern long double expl(long double x);
extern long double exp2l(long double x);
extern long double expm1l(long double x);
extern long double frexpl(long double value, int *exp);
extern int ilogbl(long double x);
extern long double ldexpl(long double x, int exp);
extern long double logl(long double x);
extern long double log10l(long double x);
extern long double log1pl(long double x);
extern long double log2l(long double x);
extern long double logbl(long double x);
extern long double modfl(long double value, long double *iptr);
extern long double scalbnl(long double x, int n);
extern long double scalblnl(long double x, long int n);

/* 7.12.7 Power and absolute-value functions */
extern long double fabsl(long double x);
extern long double hypotl(long double x, long double y);
extern long double powl(long double x, long double y);
extern long double sqrtl(long double x);

/* 7.12.8 Error and gamma functions */
extern long double erfl(long double x);
extern long double erfcl(long double x);
extern long double lgammal(long double x);
extern long double tgammal(long double x);

/* 7.12.9 Nearest integer functions */
extern long double ceill(long double x);
extern long double floorl(long double x);
extern long double nearbyintl(long double x);
extern long double rintl(long double x);
extern long int lrintl(long double x);
extern long long int llrintl(long double x);
extern long double roundl(long double x);
extern long int lroundl(long double x);
extern long long int llroundl(long double x);
extern long double truncl(long double x);

/* 7.12.10 Remainder functions */
extern long double fmodl(long double x, long double y);
extern long double remainderl(long double x, long double y);
extern long double remquol(long double x, long double y, int *quo);

/* 7.12.11 Manipulation functions */
extern long double copysignl(long double x, long double y);
extern long double nanl(const char *tagp);
extern long double nextafterl(long double x, long double y);
extern long double nexttowardl(long double x, long double y);

/* 7.12.12 Maximum, minimum, and positive difference functions */
extern long double fdiml(long double x, long double y);
extern long double fmaxl(long double x, long double y);
extern long double fminl(long double x, long double y);

/* 7.12.13 Floating multiply-add */
extern long double fmal(long double x, long double y, long double z);
#endif

/* 7.12.14 Comparison macros */
# ifndef isgreater
#  define isgreater(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && __x > __y; }))
# endif

/* Return nonzero value if X is greater than or equal to Y.  */
# ifndef isgreaterequal
#  define isgreaterequal(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && __x >= __y; }))
# endif

/* Return nonzero value if X is less than Y.  */
# ifndef isless
#  define isless(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && __x < __y; }))
# endif

/* Return nonzero value if X is less than or equal to Y.  */
# ifndef islessequal
#  define islessequal(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && __x <= __y; }))
# endif

/* Return nonzero value if either X is less than Y or Y is less than X.  */
# ifndef islessgreater
#  define islessgreater(x, y) \
  (__extension__							      \
   ({ __typeof__(x) __x = (x); __typeof__(y) __y = (y);			      \
      !isunordered (__x, __y) && (__x < __y || __y < __x); }))
# endif

/* Return nonzero value if arguments are unordered.  */
# ifndef isunordered
#  define isunordered(u, v) \
  (__extension__							      \
   ({ __typeof__(u) __u = (u); __typeof__(v) __v = (v);			      \
      fpclassify (__u) == FP_NAN || fpclassify (__v) == FP_NAN; }))
# endif


#endif /* math.h  */
