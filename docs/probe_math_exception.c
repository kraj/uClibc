/* Small test program for probing how various math functions
 * with specific operands set floating point exceptions
 */

#define _ISOC99_SOURCE 1
#define _GNU_SOURCE    1

#include <math.h>
#include <fenv.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	float infF = HUGE_VALF * 2;

	feclearexcept(FE_ALL_EXCEPT);

//	printf("%.40e\n", 1.0 / 0.0); // FE_DIVBYZERO
//	printf("%.40e\n", nextafterf(HUGE_VALF, infF)); // no exceptions in glibc 2.4

#define PREX(ex) do { if (fetestexcept(ex)) printf(#ex); } while(0)
#ifdef FE_INEXACT
	PREX(FE_INEXACT);
#endif
#ifdef FE_DIVBYZERO
	PREX(FE_DIVBYZERO);
#endif
#ifdef FE_UNDERFLOW
	PREX(FE_UNDERFLOW);
#endif
#ifdef FE_OVERFLOW
	PREX(FE_OVERFLOW);
#endif
#ifdef FE_INVALID
	PREX(FE_INVALID);
#endif
	if (fetestexcept(FE_ALL_EXCEPT))
		printf("\n");
	printf("done\n");
	return 0;
}
