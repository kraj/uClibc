/*							cmplxl.c
 *
 *	Complex number arithmetic
 *
 *
 *
 * SYNOPSIS:
 *
 * typedef struct {
 *      long double r;     real part
 *      long double i;     imaginary part
 *     }cmplxl;
 *
 * cmplxl *a, *b, *c;
 *
 * caddl( a, b, c );     c = b + a
 * csubl( a, b, c );     c = b - a
 * cmull( a, b, c );     c = b * a
 * cdivl( a, b, c );     c = b / a
 * cnegl( c );           c = -c
 * cmovl( b, c );        c = b
 *
 *
 *
 * DESCRIPTION:
 *
 * Addition:
 *    c.r  =  b.r + a.r
 *    c.i  =  b.i + a.i
 *
 * Subtraction:
 *    c.r  =  b.r - a.r
 *    c.i  =  b.i - a.i
 *
 * Multiplication:
 *    c.r  =  b.r * a.r  -  b.i * a.i
 *    c.i  =  b.r * a.i  +  b.i * a.r
 *
 * Division:
 *    d    =  a.r * a.r  +  a.i * a.i
 *    c.r  = (b.r * a.r  + b.i * a.i)/d
 *    c.i  = (b.i * a.r  -  b.r * a.i)/d
 * ACCURACY:
 *
 * In DEC arithmetic, the test (1/z) * z = 1 had peak relative
 * error 3.1e-17, rms 1.2e-17.  The test (y/z) * (z/y) = 1 had
 * peak relative error 8.3e-17, rms 2.1e-17.
 *
 * Tests in the rectangle {-10,+10}:
 *                      Relative error:
 * arithmetic   function  # trials      peak         rms
 *    DEC        cadd       10000       1.4e-17     3.4e-18
 *    IEEE       cadd      100000       1.1e-16     2.7e-17
 *    DEC        csub       10000       1.4e-17     4.5e-18
 *    IEEE       csub      100000       1.1e-16     3.4e-17
 *    DEC        cmul        3000       2.3e-17     8.7e-18
 *    IEEE       cmul      100000       2.1e-16     6.9e-17
 *    DEC        cdiv       18000       4.9e-17     1.3e-17
 *    IEEE       cdiv      100000       3.7e-16     1.1e-16
 */
/*				cmplx.c
 * complex number arithmetic
 */


/*
Cephes Math Library Release 2.3:  March, 1995
Copyright 1984, 1995 by Stephen L. Moshier
*/


#include <math.h>

/*
typedef struct
	{
	long double r;
	long double i;
	}cmplxl;
*/

#ifdef ANSIPROT
extern long double fabsl ( long double );
extern long double cabsl ( cmplxl * );
extern long double sqrtl ( long double );
extern long double atan2l ( long double, long double );
extern long double cosl ( long double );
extern long double sinl ( long double );
extern long double frexpl ( long double, int * );
extern long double ldexpl ( long double, int );
extern int isnanl ( long double );
void cdivl ( cmplxl *, cmplxl *, cmplxl * );
void caddl ( cmplxl *, cmplxl *, cmplxl * );
#else
long double fabsl(), cabsl(), sqrtl(), atan2l(), cosl(), sinl();
long double frexpl(), ldexpl();
int isnanl();
void cdivl(), caddl();
#endif


extern double MAXNUML, MACHEPL, PIL, PIO2L, INFINITYL, NANL;
cmplx czerol = {0.0L, 0.0L};
cmplx conel = {1.0L, 0.0L};


/*	c = b + a	*/

void caddl( a, b, c )
register cmplxl *a, *b;
cmplxl *c;
{

c->r = b->r + a->r;
c->i = b->i + a->i;
}


/*	c = b - a	*/

void csubl( a, b, c )
register cmplxl *a, *b;
cmplxl *c;
{

c->r = b->r - a->r;
c->i = b->i - a->i;
}

/*	c = b * a */

void cmull( a, b, c )
register cmplxl *a, *b;
cmplxl *c;
{
long double y;

y    = b->r * a->r  -  b->i * a->i;
c->i = b->r * a->i  +  b->i * a->r;
c->r = y;
}



/*	c = b / a */

void cdivl( a, b, c )
register cmplxl *a, *b;
cmplxl *c;
{
long double y, p, q, w;


y = a->r * a->r  +  a->i * a->i;
p = b->r * a->r  +  b->i * a->i;
q = b->i * a->r  -  b->r * a->i;

if( y < 1.0L )
	{
	w = MAXNUML * y;
	if( (fabsl(p) > w) || (fabsl(q) > w) || (y == 0.0L) )
		{
		c->r = INFINITYL;
		c->i = INFINITYL;
		mtherr( "cdivl", OVERFLOW );
		return;
		}
	}
c->r = p/y;
c->i = q/y;
}


/*	b = a
   Caution, a `short' is assumed to be 16 bits wide.  */

void cmovl( a, b )
void *a, *b;
{
register short *pa, *pb;
int i;

pa = (short *) a;
pb = (short *) b;
i = 16;
do
	*pb++ = *pa++;
while( --i );
}


void cnegl( a )
register cmplxl *a;
{

a->r = -a->r;
a->i = -a->i;
}

/*							cabsl()
 *
 *	Complex absolute value
 *
 *
 *
 * SYNOPSIS:
 *
 * long double cabsl();
 * cmplxl z;
 * long double a;
 *
 * a = cabs( &z );
 *
 *
 *
 * DESCRIPTION:
 *
 *
 * If z = x + iy
 *
 * then
 *
 *       a = sqrt( x**2 + y**2 ).
 * 
 * Overflow and underflow are avoided by testing the magnitudes
 * of x and y before squaring.  If either is outside half of
 * the floating point full scale range, both are rescaled.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -30,+30     30000       3.2e-17     9.2e-18
 *    IEEE      -10,+10    100000       2.7e-16     6.9e-17
 */


/*
Cephes Math Library Release 2.1:  January, 1989
Copyright 1984, 1987, 1989 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


/*
typedef struct
	{
	long double r;
	long double i;
	}cmplxl;
*/

#ifdef UNK
#define PRECL 32
#define MAXEXPL 16384
#define MINEXPL -16384
#endif
#ifdef IBMPC
#define PRECL 32
#define MAXEXPL 16384
#define MINEXPL -16384
#endif
#ifdef MIEEE
#define PRECL 32
#define MAXEXPL 16384
#define MINEXPL -16384
#endif


long double cabsl( z )
register cmplxl *z;
{
long double x, y, b, re, im;
int ex, ey, e;

#ifdef INFINITIES
/* Note, cabs(INFINITY,NAN) = INFINITY. */
if( z->r == INFINITYL || z->i == INFINITYL
   || z->r == -INFINITYL || z->i == -INFINITYL )
  return( INFINITYL );
#endif

#ifdef NANS
if( isnanl(z->r) )
  return(z->r);
if( isnanl(z->i) )
  return(z->i);
#endif

re = fabsl( z->r );
im = fabsl( z->i );

if( re == 0.0 )
	return( im );
if( im == 0.0 )
	return( re );

/* Get the exponents of the numbers */
x = frexpl( re, &ex );
y = frexpl( im, &ey );

/* Check if one number is tiny compared to the other */
e = ex - ey;
if( e > PRECL )
	return( re );
if( e < -PRECL )
	return( im );

/* Find approximate exponent e of the geometric mean. */
e = (ex + ey) >> 1;

/* Rescale so mean is about 1 */
x = ldexpl( re, -e );
y = ldexpl( im, -e );
		
/* Hypotenuse of the right triangle */
b = sqrtl( x * x  +  y * y );

/* Compute the exponent of the answer. */
y = frexpl( b, &ey );
ey = e + ey;

/* Check it for overflow and underflow. */
if( ey > MAXEXPL )
	{
	mtherr( "cabsl", OVERFLOW );
	return( INFINITYL );
	}
if( ey < MINEXPL )
	return(0.0L);

/* Undo the scaling */
b = ldexpl( b, e );
return( b );
}
/*							csqrtl()
 *
 *	Complex square root
 *
 *
 *
 * SYNOPSIS:
 *
 * void csqrtl();
 * cmplxl z, w;
 *
 * csqrtl( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 *
 * If z = x + iy,  r = |z|, then
 *
 *                       1/2
 * Im w  =  [ (r - x)/2 ]   ,
 *
 * Re w  =  y / 2 Im w.
 *
 *
 * Note that -w is also a square root of z.  The root chosen
 * is always in the upper half plane.
 *
 * Because of the potential for cancellation error in r - x,
 * the result is sharpened by doing a Heron iteration
 * (see sqrt.c) in complex arithmetic.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10     25000       3.2e-17     9.6e-18
 *    IEEE      -10,+10    100000       3.2e-16     7.7e-17
 *
 *                        2
 * Also tested by csqrt( z ) = z, and tested by arguments
 * close to the real axis.
 */


void csqrtl( z, w )
cmplxl *z, *w;
{
cmplxl q, s;
long double x, y, r, t;

x = z->r;
y = z->i;

if( y == 0.0L )
	{
	if( x < 0.0L )
		{
		w->r = 0.0L;
		w->i = sqrtl(-x);
		return;
		}
	else
		{
		w->r = sqrtl(x);
		w->i = 0.0L;
		return;
		}
	}


if( x == 0.0L )
	{
	r = fabsl(y);
	r = sqrtl(0.5L*r);
	if( y > 0.0L )
		w->r = r;
	else
		w->r = -r;
	w->i = r;
	return;
	}

/* Approximate  sqrt(x^2+y^2) - x  =  y^2/2x - y^4/24x^3 + ... .
 * The relative error in the first term is approximately y^2/12x^2 .
 */
if( (fabsl(y) < 2.e-4L * fabsl(x))
   && (x > 0) )
	{
	t = 0.25L*y*(y/x);
	}
else
	{
	r = cabsl(z);
	t = 0.5L*(r - x);
	}

r = sqrtl(t);
q.i = r;
q.r = y/(2.0L*r);
/* Heron iteration in complex arithmetic */
cdivl( &q, z, &s );
caddl( &q, &s, w );
w->r *= 0.5L;
w->i *= 0.5L;

cdivl( &q, z, &s );
caddl( &q, &s, w );
w->r *= 0.5L;
w->i *= 0.5L;
}


long double hypotl( x, y )
long double x, y;
{
cmplxl z;

z.r = x;
z.i = y;
return( cabsl(&z) );
}
