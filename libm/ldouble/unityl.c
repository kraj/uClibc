/*							unityl.c
 *
 * Relative error approximations for function arguments near
 * unity.
 *
 *    log1p(x) = log(1+x)
 *    expm1(x) = exp(x) - 1
 *    cosm1(x) = cos(x) - 1
 *
 */


/* log1p(x) = log(1 + x)
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0.5, 2      30000       1.4e-19     4.1e-20
 *
 */

#include <math.h>
/* Coefficients for log(1+x) = x - x**2/2 + x**3 P(x)/Q(x)
 * 1/sqrt(2) <= x < sqrt(2)
 * Theoretical peak relative error = 2.32e-20
 */
static long double LP[] = {
 4.5270000862445199635215E-5L,
 4.9854102823193375972212E-1L,
 6.5787325942061044846969E0L,
 2.9911919328553073277375E1L,
 6.0949667980987787057556E1L,
 5.7112963590585538103336E1L,
 2.0039553499201281259648E1L,
};
static long double LQ[] = {
/* 1.0000000000000000000000E0L,*/
 1.5062909083469192043167E1L,
 8.3047565967967209469434E1L,
 2.2176239823732856465394E2L,
 3.0909872225312059774938E2L,
 2.1642788614495947685003E2L,
 6.0118660497603843919306E1L,
};

#define SQRTH 0.70710678118654752440L
#define SQRT2 1.41421356237309504880L
#ifdef ANSIPROT
extern long double logl ( long double );
extern long double expl ( long double );
extern long double cosl ( long double );
extern long double polevll ( long double, void *, int );
extern long double p1evll ( long double,  void *, int );
#else
long double logl(), expl(), cosl(), polevll(), p1evll();
#endif

long double log1pl(x)
long double x;
{
long double z;

z = 1.0L + x;
if( (z < SQRTH) || (z > SQRT2) )
	return( logl(z) );
z = x*x;
z = -0.5L * z + x * ( z * polevll( x, LP, 6 ) / p1evll( x, LQ, 6 ) );
return (x + z);
}



/* expm1(x) = exp(x) - 1  */

/*  e^x =  1 + 2x P(x^2)/( Q(x^2) - P(x^2) )
 * -0.5 <= x <= 0.5
 */

static long double EP[3] = {
 1.2617719307481059087798E-4L,
 3.0299440770744196129956E-2L,
 9.9999999999999999991025E-1L,
};
static long double EQ[4] = {
 3.0019850513866445504159E-6L,
 2.5244834034968410419224E-3L,
 2.2726554820815502876593E-1L,
 2.0000000000000000000897E0L,
};

long double expm1l(x)
long double x;
{
long double r, xx;

if( (x < -0.5L) || (x > 0.5L) )
	return( expl(x) - 1.0L );
xx = x * x;
r = x * polevll( xx, EP, 2 );
r = r/( polevll( xx, EQ, 3 ) - r );
return (r + r);
}



/* cosm1(x) = cos(x) - 1  */

static long double coscof[7] = {
 4.7377507964246204691685E-14L,
-1.1470284843425359765671E-11L,
 2.0876754287081521758361E-9L,
-2.7557319214999787979814E-7L,
 2.4801587301570552304991E-5L,
-1.3888888888888872993737E-3L,
 4.1666666666666666609054E-2L,
};

extern long double PIO4L;

long double cosm1l(x)
long double x;
{
long double xx;

if( (x < -PIO4L) || (x > PIO4L) )
	return( cosl(x) - 1.0L );
xx = x * x;
xx = -0.5L*xx + xx * xx * polevll( xx, coscof, 6 );
return xx;
}
