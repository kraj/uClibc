#include <math.h>

int testl(long double long_double_x, int int_x, long long_x)
{
int r = 0;
r += __finitel(long_double_x);
r += __fpclassifyl(long_double_x);
r += __isinfl(long_double_x);
r += __isnanl(long_double_x);
r += __signbitl(long_double_x);
r += acoshl(long_double_x);
r += acosl(long_double_x);
r += asinhl(long_double_x);
r += asinl(long_double_x);
r += atan2l(long_double_x, long_double_x);
r += atanhl(long_double_x);
r += atanl(long_double_x);
r += cbrtl(long_double_x);
r += ceill(long_double_x);
r += copysignl(long_double_x, long_double_x);
r += coshl(long_double_x);
r += cosl(long_double_x);
r += erfcl(long_double_x);
r += erfl(long_double_x);
r += exp2l(long_double_x);
r += expl(long_double_x);
r += expm1l(long_double_x);
r += fabsl(long_double_x);
r += fdiml(long_double_x, long_double_x);
r += floorl(long_double_x);
r += fmal(long_double_x, long_double_x, long_double_x);
r += fmaxl(long_double_x, long_double_x);
r += fminl(long_double_x, long_double_x);
r += fmodl(long_double_x, long_double_x);
r += frexpl(long_double_x, &int_x);
r += hypotl(long_double_x, long_double_x);
r += ilogbl(long_double_x);
r += ldexpl(long_double_x, int_x);
r += lgammal(long_double_x);
r += llrintl(long_double_x);
r += llroundl(long_double_x);
r += log10l(long_double_x);
r += log1pl(long_double_x);
r += log2l(long_double_x);
r += logbl(long_double_x);
r += logl(long_double_x);
r += lrintl(long_double_x);
r += lroundl(long_double_x);
r += modfl(long_double_x, &long_double_x);
r += nearbyintl(long_double_x);
r += nextafterl(long_double_x, long_double_x);
/* r += nexttowardl(long_double_x, long_double_x); - uclibc doesn't provide this [yet?] */
r += powl(long_double_x, long_double_x);
r += remainderl(long_double_x, long_double_x);
r += remquol(long_double_x, long_double_x, &int_x);
r += rintl(long_double_x);
r += roundl(long_double_x);
r += scalblnl(long_double_x, long_x);
r += scalbnl(long_double_x, int_x);
r += sinhl(long_double_x);
r += sinl(long_double_x);
r += sqrtl(long_double_x);
r += tanhl(long_double_x);
r += tanl(long_double_x);
r += tgammal(long_double_x);
r += truncl(long_double_x);
return r;
}

int main(int argc, char **argv)
{
	return (long) &testl;
}
