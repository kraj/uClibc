/*							acos()
 *
 *	Inverse circular cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * double x, y, acos();
 *
 * y = acos( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns radian angle between 0 and pi whose cosine
 * is x.
 *
 * Analytically, acos(x) = pi/2 - asin(x).  However if |x| is
 * near 1, there is cancellation error in subtracting asin(x)
 * from pi/2.  Hence if x < -0.5,
 *
 *    acos(x) =	 pi - 2.0 * asin( sqrt((1+x)/2) );
 *
 * or if x > +0.5,
 *
 *    acos(x) =	 2.0 * asin(  sqrt((1-x)/2) ).
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -1, 1       50000       3.3e-17     8.2e-18
 *    IEEE      -1, 1       10^6        2.2e-16     6.5e-17
 *
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * asin domain        |x| > 1           NAN
 */

#define __USE_BSD
#include <math.h>

double acos(double x)
{
    if (x < -0.5) {
	return (M_PI - 2.0 * asin( sqrt((1+x)/2) ));
    }
    if (x > 0.5) {
	return (2.0 * asin(  sqrt((1-x)/2) ));
    }

    return(M_PI_2 - asin(x));
}
