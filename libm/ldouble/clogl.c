/*							clogl.c
 *
 *	Complex natural logarithm
 *
 *
 *
 * SYNOPSIS:
 *
 * void clogl();
 * cmplxl z, w;
 *
 * clogl( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns complex logarithm to the base e (2.718...) of
 * the complex argument x.
 *
 * If z = x + iy, r = sqrt( x**2 + y**2 ),
 * then
 *       w = log(r) + i arctan(y/x).
 * 
 * The arctangent ranges from -PI to +PI.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      7000       8.5e-17     1.9e-17
 *    IEEE      -10,+10     30000       5.0e-15     1.1e-16
 *
 * Larger relative error can be observed for z near 1 +i0.
 * In IEEE arithmetic the peak absolute error is 5.2e-16, rms
 * absolute error 1.0e-16.
 */

#include <math.h>
#ifdef ANSIPROT
static void cchshl ( long double x, long double *c, long double *s );
static long double redupil ( long double x );
static long double ctansl ( cmplxl *z );
long double cabsl ( cmplxl *x );
void csqrtl ( cmplxl *x, cmplxl *y );
void caddl ( cmplxl *x, cmplxl *y, cmplxl *z );
extern long double fabsl ( long double );
extern long double sqrtl ( long double );
extern long double logl ( long double );
extern long double expl ( long double );
extern long double atan2l ( long double, long double );
extern long double coshl ( long double );
extern long double sinhl ( long double );
extern long double asinl ( long double );
extern long double sinl ( long double );
extern long double cosl ( long double );
void clogl ( cmplxl *, cmplxl *);
void casinl ( cmplxl *, cmplxl *);
#else
static void cchshl();
static long double redupil();
static long double ctansl();
long double cabsl(), fabsl(), sqrtl();
lnog double logl(), expl(), atan2l(), coshl(), sinhl();
long double asinl(), sinl(), cosl();
void caddl(), csqrtl(), clogl(), casinl();
#endif

extern long double MAXNUML, MACHEPL, PIL, PIO2L;

void clogl( z, w )
register cmplxl *z, *w;
{
long double p, rr;

/*rr = sqrt( z->r * z->r  +  z->i * z->i );*/
rr = cabsl(z);
p = logl(rr);
#if ANSIC
rr = atan2l( z->i, z->r );
#else
rr = atan2l( z->r, z->i );
if( rr > PIL )
	rr -= PIL + PIL;
#endif
w->i = rr;
w->r = p;
}
/*							cexpl()
 *
 *	Complex exponential function
 *
 *
 *
 * SYNOPSIS:
 *
 * void cexpl();
 * cmplxl z, w;
 *
 * cexpl( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the exponential of the complex argument z
 * into the complex result w.
 *
 * If
 *     z = x + iy,
 *     r = exp(x),
 *
 * then
 *
 *     w = r cos y + i r sin y.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      8700       3.7e-17     1.1e-17
 *    IEEE      -10,+10     30000       3.0e-16     8.7e-17
 *
 */

void cexpl( z, w )
register cmplxl *z, *w;
{
long double r;

r = expl( z->r );
w->r = r * cosl( z->i );
w->i = r * sinl( z->i );
}
/*							csinl()
 *
 *	Complex circular sine
 *
 *
 *
 * SYNOPSIS:
 *
 * void csinl();
 * cmplxl z, w;
 *
 * csinl( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *
 *     w = sin x  cosh y  +  i cos x sinh y.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      8400       5.3e-17     1.3e-17
 *    IEEE      -10,+10     30000       3.8e-16     1.0e-16
 * Also tested by csin(casin(z)) = z.
 *
 */

void csinl( z, w )
register cmplxl *z, *w;
{
long double ch, sh;

cchshl( z->i, &ch, &sh );
w->r = sinl( z->r ) * ch;
w->i = cosl( z->r ) * sh;
}



/* calculate cosh and sinh */

static void cchshl( x, c, s )
long double x, *c, *s;
{
long double e, ei;

if( fabsl(x) <= 0.5L )
	{
	*c = coshl(x);
	*s = sinhl(x);
	}
else
	{
	e = expl(x);
	ei = 0.5L/e;
	e = 0.5L * e;
	*s = e - ei;
	*c = e + ei;
	}
}

/*							ccosl()
 *
 *	Complex circular cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * void ccosl();
 * cmplxl z, w;
 *
 * ccosl( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *
 *     w = cos x  cosh y  -  i sin x sinh y.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      8400       4.5e-17     1.3e-17
 *    IEEE      -10,+10     30000       3.8e-16     1.0e-16
 */

void ccosl( z, w )
register cmplxl *z, *w;
{
long double ch, sh;

cchshl( z->i, &ch, &sh );
w->r = cosl( z->r ) * ch;
w->i = -sinl( z->r ) * sh;
}
/*							ctanl()
 *
 *	Complex circular tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * void ctanl();
 * cmplxl z, w;
 *
 * ctanl( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *
 *           sin 2x  +  i sinh 2y
 *     w  =  --------------------.
 *            cos 2x  +  cosh 2y
 *
 * On the real axis the denominator is zero at odd multiples
 * of PI/2.  The denominator is evaluated by its Taylor
 * series near these points.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      5200       7.1e-17     1.6e-17
 *    IEEE      -10,+10     30000       7.2e-16     1.2e-16
 * Also tested by ctan * ccot = 1 and catan(ctan(z))  =  z.
 */

void ctanl( z, w )
register cmplxl *z, *w;
{
long double d;

d = cosl( 2.0L * z->r ) + coshl( 2.0L * z->i );

if( fabsl(d) < 0.25L )
	d = ctansl(z);

if( d == 0.0L )
	{
	mtherr( "ctan", OVERFLOW );
	w->r = MAXNUML;
	w->i = MAXNUML;
	return;
	}

w->r = sinl( 2.0L * z->r ) / d;
w->i = sinhl( 2.0L * z->i ) / d;
}
/*							ccotl()
 *
 *	Complex circular cotangent
 *
 *
 *
 * SYNOPSIS:
 *
 * void ccotl();
 * cmplxl z, w;
 *
 * ccotl( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *
 *           sin 2x  -  i sinh 2y
 *     w  =  --------------------.
 *            cosh 2y  -  cos 2x
 *
 * On the real axis, the denominator has zeros at even
 * multiples of PI/2.  Near these points it is evaluated
 * by a Taylor series.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      3000       6.5e-17     1.6e-17
 *    IEEE      -10,+10     30000       9.2e-16     1.2e-16
 * Also tested by ctan * ccot = 1 + i0.
 */

void ccotl( z, w )
register cmplxl *z, *w;
{
long double d;

d = coshl(2.0L * z->i) - cosl(2.0L * z->r);

if( fabsl(d) < 0.25L )
	d = ctansl(z);

if( d == 0.0L )
	{
	mtherr( "ccot", OVERFLOW );
	w->r = MAXNUML;
	w->i = MAXNUML;
	return;
	}

w->r = sinl( 2.0L * z->r ) / d;
w->i = -sinhl( 2.0L * z->i ) / d;
}

/* Program to subtract nearest integer multiple of PI */
/* extended precision value of PI: */
#ifdef UNK
static double DP1 = 3.14159265160560607910E0;
static double DP2 = 1.98418714791870343106E-9;
static double DP3 = 1.14423774522196636802E-17;
#endif

#ifdef DEC
static unsigned short P1[] = {0040511,0007732,0120000,0000000,};
static unsigned short P2[] = {0031010,0055060,0100000,0000000,};
static unsigned short P3[] = {0022123,0011431,0105056,0001560,};
#define DP1 *(double *)P1
#define DP2 *(double *)P2
#define DP3 *(double *)P3
#endif

#ifdef IBMPC
static unsigned short P1[] = {0x0000,0x5400,0x21fb,0x4009};
static unsigned short P2[] = {0x0000,0x1000,0x0b46,0x3e21};
static unsigned short P3[] = {0xc06e,0x3145,0x6263,0x3c6a};
#define DP1 *(double *)P1
#define DP2 *(double *)P2
#define DP3 *(double *)P3
#endif

#ifdef MIEEE
static unsigned short P1[] = {
0x4009,0x21fb,0x5400,0x0000
};
static unsigned short P2[] = {
0x3e21,0x0b46,0x1000,0x0000
};
static unsigned short P3[] = {
0x3c6a,0x6263,0x3145,0xc06e
};
#define DP1 *(double *)P1
#define DP2 *(double *)P2
#define DP3 *(double *)P3
#endif

static long double redupil(x)
long double x;
{
long double t;
long i;

t = x/PIL;
if( t >= 0.0L )
	t += 0.5L;
else
	t -= 0.5L;

i = t;	/* the multiple */
t = i;
t = ((x - t * DP1) - t * DP2) - t * DP3;
return(t);
}

/*  Taylor series expansion for cosh(2y) - cos(2x)	*/

static long double ctansl(z)
cmplxl *z;
{
long double f, x, x2, y, y2, rn, t;
long double d;

x = fabsl( 2.0L * z->r );
y = fabsl( 2.0L * z->i );

x = redupil(x);

x = x * x;
y = y * y;
x2 = 1.0L;
y2 = 1.0L;
f = 1.0L;
rn = 0.0;
d = 0.0;
do
	{
	rn += 1.0L;
	f *= rn;
	rn += 1.0L;
	f *= rn;
	x2 *= x;
	y2 *= y;
	t = y2 + x2;
	t /= f;
	d += t;

	rn += 1.0L;
	f *= rn;
	rn += 1.0L;
	f *= rn;
	x2 *= x;
	y2 *= y;
	t = y2 - x2;
	t /= f;
	d += t;
	}
while( fabsl(t/d) > MACHEPL );
return(d);
}
/*							casinl()
 *
 *	Complex circular arc sine
 *
 *
 *
 * SYNOPSIS:
 *
 * void casinl();
 * cmplxl z, w;
 *
 * casinl( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * Inverse complex sine:
 *
 *                               2
 * w = -i clog( iz + csqrt( 1 - z ) ).
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10     10100       2.1e-15     3.4e-16
 *    IEEE      -10,+10     30000       2.2e-14     2.7e-15
 * Larger relative error can be observed for z near zero.
 * Also tested by csin(casin(z)) = z.
 */

void casinl( z, w )
cmplxl *z, *w;
{
static cmplxl ca, ct, zz, z2;
long double x, y;

x = z->r;
y = z->i;

if( y == 0.0L )
	{
	if( fabsl(x) > 1.0L )
		{
		w->r = PIO2L;
		w->i = 0.0L;
		mtherr( "casinl", DOMAIN );
		}
	else
		{
		w->r = asinl(x);
		w->i = 0.0L;
		}
	return;
	}

/* Power series expansion */
/*
b = cabsl(z);
if( b < 0.125L )
{
z2.r = (x - y) * (x + y);
z2.i = 2.0L * x * y;

cn = 1.0L;
n = 1.0L;
ca.r = x;
ca.i = y;
sum.r = x;
sum.i = y;
do
	{
	ct.r = z2.r * ca.r  -  z2.i * ca.i;
	ct.i = z2.r * ca.i  +  z2.i * ca.r;
	ca.r = ct.r;
	ca.i = ct.i;

	cn *= n;
	n += 1.0L;
	cn /= n;
	n += 1.0L;
	b = cn/n;

	ct.r *= b;
	ct.i *= b;
	sum.r += ct.r;
	sum.i += ct.i;
	b = fabsl(ct.r) + fabs(ct.i);
	}
while( b > MACHEPL );
w->r = sum.r;
w->i = sum.i;
return;
}
*/


ca.r = x;
ca.i = y;

ct.r = -ca.i;	/* iz */
ct.i = ca.r;

	/* sqrt( 1 - z*z) */
/* cmul( &ca, &ca, &zz ) */
zz.r = (ca.r - ca.i) * (ca.r + ca.i);	/*x * x  -  y * y */
zz.i = 2.0L * ca.r * ca.i;

zz.r = 1.0L - zz.r;
zz.i = -zz.i;
csqrtl( &zz, &z2 );

caddl( &z2, &ct, &zz );
clogl( &zz, &zz );
w->r = zz.i;	/* mult by 1/i = -i */
w->i = -zz.r;
return;
}
/*							cacosl()
 *
 *	Complex circular arc cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * void cacosl();
 * cmplxl z, w;
 *
 * cacosl( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 *
 * w = arccos z  =  PI/2 - arcsin z.
 *
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      5200      1.6e-15      2.8e-16
 *    IEEE      -10,+10     30000      1.8e-14      2.2e-15
 */

void cacosl( z, w )
cmplxl *z, *w;
{

casinl( z, w );
w->r = PIO2L  -  w->r;
w->i = -w->i;
}
/*							catanl()
 *
 *	Complex circular arc tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * void catanl();
 * cmplxl z, w;
 *
 * catanl( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *          1       (    2x     )
 * Re w  =  - arctan(-----------)  +  k PI
 *          2       (     2    2)
 *                  (1 - x  - y )
 *
 *               ( 2         2)
 *          1    (x  +  (y+1) )
 * Im w  =  - log(------------)
 *          4    ( 2         2)
 *               (x  +  (y-1) )
 *
 * Where k is an arbitrary integer.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      5900       1.3e-16     7.8e-18
 *    IEEE      -10,+10     30000       2.3e-15     8.5e-17
 * The check catan( ctan(z) )  =  z, with |x| and |y| < PI/2,
 * had peak relative error 1.5e-16, rms relative error
 * 2.9e-17.  See also clog().
 */

void catanl( z, w )
cmplxl *z, *w;
{
long double a, t, x, x2, y;

x = z->r;
y = z->i;

if( (x == 0.0L) && (y > 1.0L) )
	goto ovrf;

x2 = x * x;
a = 1.0L - x2 - (y * y);
if( a == 0.0L )
	goto ovrf;

#if ANSIC
t = atan2l( 2.0L * x, a ) * 0.5L;
#else
t = atan2l( a, 2.0 * x ) * 0.5L;
#endif
w->r = redupil( t );

t = y - 1.0L;
a = x2 + (t * t);
if( a == 0.0L )
	goto ovrf;

t = y + 1.0L;
a = (x2 + (t * t))/a;
w->i = logl(a)/4.0;
return;

ovrf:
mtherr( "catanl", OVERFLOW );
w->r = MAXNUML;
w->i = MAXNUML;
}
