#include <math.h>

void testl(long double long_double_x, int int_x, long long_x)
{
__finitel(long_double_x);
__fpclassifyl(long_double_x);
__isinfl(long_double_x);
__isnanl(long_double_x);
__signbitl(long_double_x);
acoshl(long_double_x);
acosl(long_double_x);
asinhl(long_double_x);
asinl(long_double_x);
atan2l(long_double_x, long_double_x);
atanhl(long_double_x);
atanl(long_double_x);
cbrtl(long_double_x);
ceill(long_double_x);
copysignl(long_double_x, long_double_x);
coshl(long_double_x);
cosl(long_double_x);
erfcl(long_double_x);
erfl(long_double_x);
exp2l(long_double_x);
expl(long_double_x);
expm1l(long_double_x);
fabsl(long_double_x);
fdiml(long_double_x, long_double_x);
floorl(long_double_x);
fmal(long_double_x, long_double_x, long_double_x);
fmaxl(long_double_x, long_double_x);
fminl(long_double_x, long_double_x);
fmodl(long_double_x, long_double_x);
frexpl(long_double_x, &int_x);
hypotl(long_double_x, long_double_x);
ilogbl(long_double_x);
ldexpl(long_double_x, int_x);
lgammal(long_double_x);
llrintl(long_double_x);
llroundl(long_double_x);
log10l(long_double_x);
log1pl(long_double_x);
log2l(long_double_x);
logbl(long_double_x);
logl(long_double_x);
lrintl(long_double_x);
lroundl(long_double_x);
modfl(long_double_x, &long_double_x);
nearbyintl(long_double_x);
nextafterl(long_double_x, long_double_x);
nexttowardl(long_double_x, long_double_x);
powl(long_double_x, long_double_x);
remainderl(long_double_x, long_double_x);
remquol(long_double_x, long_double_x, &int_x);
rintl(long_double_x);
roundl(long_double_x);
scalblnl(long_double_x, long_x);
scalbnl(long_double_x, int_x);
sinhl(long_double_x);
sinl(long_double_x);
sqrtl(long_double_x);
tanhl(long_double_x);
tanl(long_double_x);
tgammal(long_double_x);
truncl(long_double_x);
}

int main(int argc, char **argv)
{
	return (long) &testl;
}
