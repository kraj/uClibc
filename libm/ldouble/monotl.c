
/* monot.c
   Floating point function test vectors.

   Arguments and function values are synthesized for NPTS points in
   the vicinity of each given tabulated test point.  The points are
   chosen to be near and on either side of the likely function algorithm
   domain boundaries.  Since the function programs change their methods
   at these points, major coding errors or monotonicity failures might be
   detected.

   August, 1998
   S. L. Moshier  */


#include <stdio.h>

/* Avoid including math.h.  */
long double frexpl (long double, int *);
long double ldexpl (long double, int);

/* Number of test points to generate on each side of tabulated point.  */
#define NPTS 100

/* Functions of one variable.  */
long double expl (long double);
long double logl (long double);
long double sinl (long double);
long double cosl (long double);
long double tanl (long double);
long double atanl (long double);
long double asinl (long double);
long double acosl (long double);
long double sinhl (long double);
long double coshl (long double);
long double tanhl (long double);
long double asinhl (long double);
long double acoshl (long double);
long double atanhl (long double);
long double gammal (long double);
long double fabsl (long double);
long double floorl (long double);

struct oneargument
  {
    char *name;			/* Name of the function. */
    long double (*func) (long double);
    long double arg1;		/* Function argument, assumed exact.  */
    long double answer1;	/* Exact, close to function value.  */
    long double answer2;	/* answer1 + answer2 has extended precision. */
    long double derivative;	/* dy/dx evaluated at x = arg1. */
    int thresh;			/* Error report threshold. 2 = 1 ULP approx. */
  };

/* Add this to error threshold test[i].thresh.  */
#define OKERROR 2

/* Unit of relative error in test[i].thresh.  */
static long double MACHEPL = 5.42101086242752217003726400434970855712890625E-20L;

/* extern double MACHEP; */


struct oneargument test1[] =
{
  {"exp", expl, 1.0L, 2.7182769775390625L,
   4.85091998273536028747e-6L, 2.71828182845904523536L, 1},
  {"exp", expl, -1.0L, 3.678741455078125e-1L,
    5.29566362982159552377e-6L, 3.678794411714423215955e-1L, 1},
  {"exp", expl, 0.5L, 1.648712158203125L,
    9.1124970031468486507878e-6L, 1.64872127070012814684865L, 1},
  {"exp", expl, -0.5L, 6.065216064453125e-1L,
    9.0532673209236037995e-6L, 6.0653065971263342360e-1L, 1},
  {"exp", expl, 2.0L, 7.3890533447265625L,
    2.75420408772723042746e-6L, 7.38905609893065022723L, 1},
  {"exp", expl, -2.0L, 1.353302001953125e-1L,
    5.08304130019189399949e-6L, 1.3533528323661269189e-1L, 1},
  {"log", logl, 1.41421356237309492343L, 3.465728759765625e-1L,
   7.1430341006605745676897e-7L, 7.0710678118654758708668e-1L, 1},
  {"log", logl, 7.07106781186547461715e-1L, -3.46588134765625e-1L,
   1.45444856522566402246e-5L, 1.41421356237309517417L, 1},
  {"sin", sinl, 7.85398163397448278999e-1L, 7.0709228515625e-1L,
   1.4496030297502751942956e-5L, 7.071067811865475460497e-1L, 1},
  {"sin", sinl, -7.85398163397448501044e-1L, -7.071075439453125e-1L,
   7.62758764840238811175e-7L, 7.07106781186547389040e-1L, 1},
  {"sin", sinl, 1.570796326794896558L, 9.999847412109375e-1L,
   1.52587890625e-5L, 6.12323399573676588613e-17L, 1},
  {"sin", sinl, -1.57079632679489678004L, -1.0L,
   1.29302922820150306903e-32L, -1.60812264967663649223e-16L, 1},
  {"sin", sinl, 4.712388980384689674L, -1.0L,
   1.68722975549458979398e-32L, -1.83697019872102976584e-16L, 1},
  {"sin", sinl, -4.71238898038468989604L, 9.999847412109375e-1L,
   1.52587890625e-5L, 3.83475850529283315008e-17L, 1},
  {"cos", cosl, 3.92699081698724139500E-1L, 9.23873901367187500000E-1L,
   5.63114409926198633370E-6L, -3.82683432365089757586E-1L, 1},
  {"cos", cosl, 7.85398163397448278999E-1L, 7.07092285156250000000E-1L,
   1.44960302975460497458E-5L, -7.07106781186547502752E-1L, 1},
  {"cos", cosl, 1.17809724509617241850E0L, 3.82675170898437500000E-1L,
   8.26146665231415693919E-6L, -9.23879532511286738554E-1L, 1},
  {"cos", cosl, 1.96349540849362069750E0L, -3.82690429687500000000E-1L,
   6.99732241029898567203E-6L, -9.23879532511286785419E-1L, 1},
  {"cos", cosl, 2.35619449019234483700E0L, -7.07107543945312500000E-1L,
   7.62758765040545859856E-7L, -7.07106781186547589348E-1L, 1},
  {"cos", cosl, 2.74889357189106897650E0L, -9.23889160156250000000E-1L,
   9.62764496328487887036E-6L, -3.82683432365089870728E-1L, 1},
  {"cos", cosl, 3.14159265358979311600E0L, -1.00000000000000000000E0L,
   7.49879891330928797323E-33L, -1.22464679914735317723E-16L, 1},
  {"tan", tanl, 7.85398163397448278999E-1L, 9.999847412109375e-1L,
   1.52587890624387676600E-5L, 1.99999999999999987754E0L, 1},
  {"tan", tanl, 1.17809724509617241850E0L, 2.41419982910156250000E0L,
   1.37332715322352112604E-5L, 6.82842712474618858345E0L, 1},
  {"tan", tanl, 1.96349540849362069750E0L, -2.41421508789062500000E0L,
   1.52551752942854759743E-6L, 6.82842712474619262118E0L, 1},
  {"tan", tanl, 2.35619449019234483700E0L, -1.00001525878906250000E0L,
   1.52587890623163029801E-5L, 2.00000000000000036739E0L, 1},
  {"tan", tanl, 2.74889357189106897650E0L, -4.14215087890625000000E-1L,
   1.52551752982565655126E-6L, 1.17157287525381000640E0L, 1},
  {"atan", atanl, 4.14213562373094923430E-1L, 3.92684936523437500000E-1L,
   1.41451752865477964149E-5L, 8.53553390593273837869E-1L, 1},
  {"atan", atanl, 1.0L, 7.85385131835937500000E-1L,
   1.30315615108096156608E-5L, 0.5L, 1},
  {"atan", atanl, 2.41421356237309492343E0L, 1.17808532714843750000E0L,
   1.19179477349460632350E-5L, 1.46446609406726250782E-1L, 1},
  {"atan", atanl, -2.41421356237309514547E0L, -1.17810058593750000000E0L,
   3.34084132752141908545E-6L, 1.46446609406726227789E-1L, 1},
  {"atan", atanl, -1.0L, -7.85400390625000000000E-1L,
   2.22722755169038433915E-6L, 0.5L, 1},
  {"atan", atanl, -4.14213562373095145475E-1L, -3.92700195312500000000E-1L,
   1.11361377576267665972E-6L, 8.53553390593273703853E-1L, 1},
  {"asin", asinl, 3.82683432365089615246E-1L, 3.92684936523437500000E-1L,
   1.41451752864854321970E-5L, 1.08239220029239389286E0L, 1},
  {"asin", asinl, 0.5L, 5.23590087890625000000E-1L,
   8.68770767387307710723E-6L, 1.15470053837925152902E0L, 1},
  {"asin", asinl, 7.07106781186547461715E-1L, 7.85385131835937500000E-1L,
   1.30315615107209645016E-5L, 1.41421356237309492343E0L, 1},
  {"asin", asinl, 9.23879532511286738483E-1L, 1.17808532714843750000E0L,
   1.19179477349183147612E-5L, 2.61312592975275276483E0L, 1},
  {"asin", asinl, -0.5L, -5.23605346679687500000E-1L,
   6.57108138862692289277E-6L, 1.15470053837925152902E0L, 1},
  {"acos", acosl, 1.95090322016128192573E-1L, 1.37443542480468750000E0L,
   1.13611408471185777914E-5L, -1.01959115820831832232E0L, 1},
  {"acos", acosl, 3.82683432365089615246E-1L, 1.17808532714843750000E0L,
   1.19179477351337991247E-5L, -1.08239220029239389286E0L, 1},
  {"acos", acosl, 0.5L, 1.04719543457031250000E0L,
   2.11662628524615421446E-6L, -1.15470053837925152902E0L, 1},
  {"acos", acosl, 7.07106781186547461715E-1L, 7.85385131835937500000E-1L,
   1.30315615108982668201E-5L, -1.41421356237309492343E0L, 1},
  {"acos", acosl, 9.23879532511286738483E-1L, 3.92684936523437500000E-1L,
   1.41451752867009165605E-5L, -2.61312592975275276483E0L, 1},
  {"acos", acosl, 9.80785280403230430579E-1L, 1.96334838867187500000E-1L,
   1.47019821746724723933E-5L, -5.12583089548300990774E0L, 1},
  {"acos", acosl, -0.5L, 2.09439086914062500000E0L,
   4.23325257049230842892E-6L, -1.15470053837925152902E0L, 1},
  {"sinh", sinhl, 1.0L, 1.17518615722656250000E0L,
   1.50364172389568823819E-5L, 1.54308063481524377848E0L, 1},
  {"sinh", sinhl, 7.09089565712818057364E2L, 4.49423283712885057274E307L,
   4.25947714184369757620E208L, 4.49423283712885057274E307L, 1},
  {"sinh", sinhl, 2.22044604925031308085E-16L, 0.00000000000000000000E0L,
   2.22044604925031308085E-16L, 1.00000000000000000000E0L, 1},
  {"cosh", coshl, 7.09089565712818057364E2L, 4.49423283712885057274E307L,
   4.25947714184369757620E208L, 4.49423283712885057274E307L, 1},
  {"cosh", coshl, 1.0L, 1.54307556152343750000E0L,
   5.07329180627847790562E-6L, 1.17520119364380145688E0L, 1},
  {"cosh", coshl, 0.5L, 1.12762451171875000000E0L,
   1.45348763078522622516E-6L, 5.21095305493747361622E-1L, 1},
  {"tanh", tanhl, 0.5L, 4.62112426757812500000E-1L,
   4.73050219725850231848E-6L, 7.86447732965927410150E-1L, 1},
  {"tanh", tanhl, 5.49306144334054780032E-1L, 4.99984741210937500000E-1L,
   1.52587890624507506378E-5L, 7.50000000000000049249E-1L, 1},
  {"tanh", tanhl, 0.625L, 5.54595947265625000000E-1L,
   3.77508375729399903910E-6L, 6.92419147969988069631E-1L, 1},
  {"asinh", asinhl, 0.5L, 4.81201171875000000000E-1L,
   1.06531846034474977589E-5L, 8.94427190999915878564E-1L, 1},
  {"asinh", asinhl, 1.0L, 8.81362915039062500000E-1L,
   1.06719804805252326093E-5L, 7.07106781186547524401E-1L, 1},
  {"asinh", asinhl, 2.0L, 1.44363403320312500000E0L,
   1.44197568534249327674E-6L, 4.47213595499957939282E-1L, 1},
  {"acosh", acoshl, 2.0L, 1.31695556640625000000E0L,
   2.33051856670862504635E-6L, 5.77350269189625764509E-1L, 1},
  {"acosh", acoshl, 1.5L, 9.62417602539062500000E-1L,
   6.04758014439499551783E-6L, 8.94427190999915878564E-1L, 1},
  {"acosh", acoshl, 1.03125L, 2.49343872070312500000E-1L,
   9.62177257298785143908E-6L, 3.96911150685467059809E0L, 1},
  {"atanh", atanhl, 0.5L, 5.49301147460937500000E-1L,
   4.99687311734569762262E-6L, 1.33333333333333333333E0L, 1},
#if 0
  {"gamma", gammal, 1.0L, 1.0L,
   0.0L, -5.772156649015328606e-1L, 1},
  {"gamma", gammal, 2.0L, 1.0L,
   0.0L, 4.2278433509846713939e-1L, 1},
  {"gamma", gammal, 3.0L, 2.0L,
   0.0L, 1.845568670196934279L, 1},
  {"gamma", gammal, 4.0L, 6.0L,
   0.0L, 7.536706010590802836L, 1},
#endif
  {"null", NULL, 0.0L, 0.0L, 0.0L, 1},
};

/* These take care of extra-precise floating point register problems.  */
volatile long double volat1;
volatile long double volat2;


/* Return the next nearest floating point value to X
   in the direction of UPDOWN (+1 or -1).
   (Fails if X is denormalized.)  */

long double
nextval (x, updown)
     long double x;
     int updown;
{
  long double m;
  int i;

  volat1 = x;
  m = 0.25L * MACHEPL * volat1 * updown;
  volat2 = volat1 + m;
  if (volat2 != volat1)
    printf ("successor failed\n");

  for (i = 2; i < 10; i++)
    {
      volat2 = volat1 + i * m;
      if (volat1 != volat2)
	return volat2;
    }

  printf ("nextval failed\n");
  return volat1;
}




int
main ()
{
  long double (*fun1) (long double);
  int i, j, errs, tests;
  long double x, x0, y, dy, err;

  errs = 0;
  tests = 0;
  i = 0;

  for (;;)
    {
      fun1 = test1[i].func;
      if (fun1 == NULL)
	break;
      volat1 = test1[i].arg1;
      x0 = volat1;
      x = volat1;
      for (j = 0; j <= NPTS; j++)
	{
	  volat1 = x - x0;
	  dy = volat1 * test1[i].derivative;
	  dy = test1[i].answer2 + dy;
	  volat1 = test1[i].answer1 + dy;
	  volat2 = (*(fun1)) (x);
	  if (volat2 != volat1)
	    {
	      /* Report difference between program result
		 and extended precision function value.  */
	      err = volat2 - test1[i].answer1;
	      err = err - dy;
	      err = err / volat1;
	      if (fabsl (err) > ((OKERROR + test1[i].thresh) * MACHEPL))
		{
		  printf ("%d %s(%.19Le) = %.19Le, rel err = %.3Le\n",
			  j, test1[i].name, x, volat2, err);
		  errs += 1;
		}
	    }
	  x = nextval (x, 1);
	  tests += 1;
	}

      x = x0;
      x = nextval (x, -1);
      for (j = 1; j < NPTS; j++)
	{
	  volat1 = x - x0;
	  dy = volat1 * test1[i].derivative;
	  dy = test1[i].answer2 + dy;
	  volat1 = test1[i].answer1 + dy;
	  volat2 = (*(fun1)) (x);
	  if (volat2 != volat1)
	    {
	      err = volat2 - test1[i].answer1;
	      err = err - dy;
	      err = err / volat1;
	      if (fabsl (err) > ((OKERROR + test1[i].thresh) * MACHEPL))
		{
		  printf ("%d %s(%.19Le) = %.19Le, rel err = %.3Le\n",
			  j, test1[i].name, x, volat2, err);
		  errs += 1;
		}
	    }
	  x = nextval (x, -1);
	  tests += 1;
	}
      i += 1;
    }
  printf ("%d errors in %d tests\n", errs, tests);
}
