#define _XOPEN_SOURCE 600
#include <math.h>
#include <stdio.h>

double zero = 0.0;
double mzero;

int
main (void)
{
  double d;
  mzero = copysign (zero, -1.0);
  d = lgamma (zero);
  printf ("%g %d\n", d, signgam);
  d = lgamma (mzero);
  printf ("%g %d\n", d, signgam);
  return 0;
}
