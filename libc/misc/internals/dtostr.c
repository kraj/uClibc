/*
 * Copyright (C) 2000, 2001 Manuel Novoa III
 *
 * Function:  int __dtostr(FILE * fp, size_t size, double x, 
 *			               char flag[], int width, int preci, char mode)
 *
 * This was written for uClibc to provide floating point support for
 * the printf functions.  It handles +/- infinity and nan on i386.
 *
 * Notes:
 *
 * It should also be fairly portable, as not assumptions are made about the
 * bit-layout of doubles.
 *
 * It should be too difficult to convert this to handle long doubles on i386.
 * For information, see the comments below.
 *
 * TODO: 
 *   long double and/or float version?  (note: for float can trim code some).
 *   
 *   Decrease the size.  This is really much bigger than I'd like.
 */

/*****************************************************************************/
/* Don't change anything that follows unless you know what you're doing.     */
/*****************************************************************************/

/*
 * Configuration for the scaling power table.  Ignoring denormals, you
 * should have 2**EXP_TABLE_SIZE >= MAX_DBL_EXP >= 2**(EXP_TABLE_SIZE-1).
 * The minimum for standard C is 6.  For IEEE 8bit doubles, 9 suffices.
 */
#define EXP_TABLE_SIZE       9

/* 
 * Set this to the maximum number of digits you want converted.
 * Conversion is done in blocks of DIGITS_PER_BLOCK (9 by default) digits.
 * 17 digits suffices to uniquely determine a double on i386.
 */
#define MAX_DIGITS          17

/*
 * Set this to the smallest integer type capable of storing a pointer.
 */
#define INT_OR_PTR int

/*
 * This is really only used to check for infinities.  The macro produces
 * smaller code for i386 and, since this is tested before any floating point
 * calculations, it doesn't appear to suffer from the excess precision problem
 * caused by the FPU that strtod had.  If it causes problems, call the function
 * and compile zoicheck.c with -ffloat-store.
 */
#if 1
#define _zero_or_inf_check(x) ( x == (x/4) )
#else
extern int _zero_or_inf_check(double x);
#endif

/*
 * Fairly portable nan check.  Bitwise for i386 generated larger code.
 * If you have a better version, comment this out.
 */
#define isnan(x) (x != x)

/*****************************************************************************/
/* Don't change anything that follows peroid!!!  ;-)                         */
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <limits.h>

extern int fnprintf(FILE * fp, size_t size, const char *fmt, ...);

/* from printf.c -- should really be in an internal header file */
enum {
	FLAG_PLUS = 0,
	FLAG_MINUS_LJUSTIFY,
	FLAG_HASH,
	FLAG_0_PAD,
	FLAG_SPACE,
};

/*****************************************************************************/

/*
 * Set things up for the scaling power table.
 */

#if EXP_TABLE_SIZE < 6
#error EXP_TABLE_SIZE should be at least 6 to comply with standards
#endif

#define EXP_TABLE_MAX      (1U<<(EXP_TABLE_SIZE-1))

/*
 * Only bother checking if this is too small.
 * Throw in some play for denormals ( roughly O(-324) vs O(-307) on i386 ).
 */

#if (3+DBL_DIG-DBL_MIN_10_EXP)/2 > EXP_TABLE_MAX
#error larger EXP_TABLE_SIZE needed
#endif

/*
 * With 32 bit ints, we can get 9 digits per block.
 */
#define DIGITS_PER_BLOCK     9

#if (INT_MAX >> 30)
#define DIGIT_BLOCK_TYPE     int
#define DB_FMT               "%.*d"
#elif (LONG_MAX >> 30)
#define DIGIT_BLOCK_TYPE     long
#define DB_FMT               "%.*ld"
#else
#error need at least 32 bit longs
#endif

/* Are there actually any machines where this might fail? */
#if 'A' > 'a'
#error ordering assumption violated : 'A' > 'a'
#endif

/* Maximum number of calls to fnprintf to output double. */
#define MAX_CALLS 8

/*****************************************************************************/

#define NUM_DIGIT_BLOCKS   ((MAX_DIGITS+DIGITS_PER_BLOCK-1)/DIGITS_PER_BLOCK)

/* extra space for '-', '.', 'e+###', and nul */
#define BUF_SIZE  ( 3 + NUM_DIGIT_BLOCKS * DIGITS_PER_BLOCK )
/*****************************************************************************/

static const char *fmts[] = {
	"%0*d", "%.*s", ".", "inf", "INF", "nan", "NAN", "%*s"
};

/*****************************************************************************/

int __dtostr(FILE * fp, size_t size, double x, 
			 char flag[], int width, int preci, char mode)
{
	double exp_table[EXP_TABLE_SIZE];
	double p10;
	DIGIT_BLOCK_TYPE digit_block; /* int of at least 32 bits */
	int i, j;
	int round, o_exp;
	int exp, exp_neg;
	char *s;
	char *e;
	char buf[BUF_SIZE];
	char buf2[BUF_SIZE];
	INT_OR_PTR pc_fwi[2*MAX_CALLS];
	INT_OR_PTR *ppc;
	char exp_buf[8];
	char drvr[8];
	char *pdrvr;
	int npc;
	int cnt;
	char sign_str[2];
	char o_mode;

	/* check that INT_OR_PTR is sufficiently large */
	assert( sizeof(INT_OR_PTR) == sizeof(char *) );

	*sign_str = flag[FLAG_PLUS];
	*(sign_str+1) = 0;
	if (isnan(x)) {				/* nan check */
		pdrvr = drvr + 1;
		*pdrvr++ = 5 + (mode < 'a');
		pc_fwi[2] = 3;
		flag[FLAG_0_PAD] = 0;
		goto EXIT_SPECIAL;
	}

	if (x == 0) {				/* handle 0 now to avoid false positive */
		exp = -1;
		goto GENERATE_DIGITS;
	}

	if (x < 0) {				/* convert negatives to positives */
		*sign_str = '-';
		x = -x;
	}

	if (_zero_or_inf_check(x)) { /* must be inf since zero handled above */
		pdrvr = drvr + 1;
		*pdrvr++ = 3 +  + (mode < 'a');
		pc_fwi[2] = 3;
		flag[FLAG_0_PAD] = 0;
		goto EXIT_SPECIAL;
	}

	/* need to build the scaling table */
	for (i = 0, p10 = 10 ; i < EXP_TABLE_SIZE ; i++) {
		exp_table[i] = p10;
		p10 *= p10;
	}

	exp_neg = 0;
	if (x < 1e8) {				/* do we need to scale up or down? */
		exp_neg = 1;
	}

	exp = DIGITS_PER_BLOCK - 1;

	i = EXP_TABLE_SIZE;
	j = EXP_TABLE_MAX;
	while ( i-- ) {				/* scale x such that 1e8 <= x < 1e9 */
		if (exp_neg) {
			if (x * exp_table[i] < 1e9) {
				x *= exp_table[i];
				exp -= j;
			}
		} else {
			if (x / exp_table[i] >= 1e8) {
				x /= exp_table[i];
				exp += j;
			}
		}
		j >>= 1;
	}
	if (x >= 1e9) {				/* handle bad rounding case */
		x /= 10;
		++exp;
	}
	assert(x < 1e9);

 GENERATE_DIGITS:
	s = buf2 + 2; /* leave space for '\0' and '0' */

	for (i = 0 ; i < NUM_DIGIT_BLOCKS ; ++i ) {
		digit_block = (DIGIT_BLOCK_TYPE) x;
		x = (x - digit_block) * 1e9;
		s += sprintf(s, DB_FMT, DIGITS_PER_BLOCK, digit_block);
	}

	/*************************************************************************/

	*exp_buf = 'e';
	if (mode < 'a') {
		*exp_buf = 'E';
		mode += ('a' - 'A');
	} 

	o_mode = mode;

	round = preci;

	if ((mode == 'g') && (round > 0)){
		--round;
	}

	if (mode == 'f') {
		round += exp;
	}

 RESTART:
	memcpy(buf,buf2,sizeof(buf2)); /* backup in case g need to be f */

	s = buf;
	*s++ = 0;					/* terminator for rounding and 0-triming */
	*s = '0';					/* space to round */

	i = 0;
	e = s + MAX_DIGITS + 1;
	if (round < MAX_DIGITS) {
		e = s + round + 2;
		if (*e >= '5') {
			i = 1;
		}
	}

	do {						/* handle rounding and trim trailing 0s */
		*--e += i;				/* add the carry */
	} while ((*e == '0') || (*e > '9'));

	o_exp = exp;
	if (e <= s) {				/* we carried into extra digit */
		++o_exp;
		e = s;					/* needed if all 0s */
	} else {
		++s;
	}
	*++e = 0;					/* ending nul char */

	if ((mode == 'g') && ((o_exp >= -4) && (o_exp < round))) {
		mode = 'f';
		goto RESTART;
	}

	exp = o_exp;
	if (mode != 'f') {
		o_exp = 0;
	}

	if (o_exp < 0) {
		*--s = '0';				/* fake the first digit */
	}

	pdrvr = drvr+1;
	ppc = pc_fwi+2;

	*pdrvr++ = 0;
	*ppc++ = 1;
	*ppc++ = (INT_OR_PTR)(*s++ - '0');

	i = e - s;					/* total digits */
	if (o_exp >= 0) {
		if (o_exp >= i) {		/* all digit(s) left of decimal */
			*pdrvr++ = 1;
			*ppc++ = i;
			*ppc++ = (INT_OR_PTR)(s);
			o_exp -= i;
			i = 0;
			if (o_exp>0) {		/* have 0s left of decimal */
				*pdrvr++ = 0;
				*ppc++ = o_exp;
				*ppc++ = 0;
			}
		} else if (o_exp > 0) {	/* decimal between digits */
			*pdrvr++ = 1;
			*ppc++ = o_exp;
			*ppc++ = (INT_OR_PTR)(s);
			s += o_exp;
			i -= o_exp;
		}
		o_exp = -1;
	}

	if (flag[FLAG_HASH] || (i) || ((o_mode != 'g') && (preci > 0))) {
		*pdrvr++ = 2;			/* need decimal */
		*ppc++ = 1;				/* needed for width calc */
		ppc++;
	}

	if (++o_exp < 0) {			/* have 0s right of decimal */
		*pdrvr++ = 0;
		*ppc++ = -o_exp;
		*ppc++ = 0;
	}
	if (i) {					/* have digit(s) right of decimal */
		*pdrvr++ = 1;
		*ppc++ = i;
		*ppc++ = (INT_OR_PTR)(s);
	}

	if (o_mode != 'g') {
		i -= o_exp;
		if (i < preci) {		/* have 0s right of digits */
			i = preci - i;
			*pdrvr++ = 0;
			*ppc++ = i;
			*ppc++ = 0;
		}
	}

	/* build exponent string */
	if (mode != 'f') {
		*pdrvr++ = 1;
		*ppc++ = sprintf(exp_buf,"%c%+.2d", *exp_buf, exp);
		*ppc++ = (INT_OR_PTR) exp_buf;
	}

 EXIT_SPECIAL:
	npc = pdrvr - drvr;
	ppc = pc_fwi + 2;
	for (i=1 ; i< npc ; i++) {
		width -= *(ppc++);
		ppc++;
	}
	i = 0;
	if (*sign_str) {
		i = 1;
	}
	width -= i;
	if (width <= 0) {
		width = 0;
	} else {
		if (flag[FLAG_MINUS_LJUSTIFY]) { /* padding on right */
			++npc;
			*pdrvr++ = 7;
			*ppc = width;
			*++ppc = (INT_OR_PTR)("");
			width = 0;
		} else if (flag[FLAG_0_PAD] == '0') { /* 0 padding */
			pc_fwi[2] += width;
			width = 0;
		}
	}
	*drvr = 7;
	ppc = pc_fwi;
	*ppc++ = width + i;
	*ppc = (INT_OR_PTR) sign_str;

	pdrvr = drvr;
	ppc = pc_fwi;
	cnt = 0;
	for (i=0 ; i<npc ; i++) {
#if 1
		fnprintf(fp, size, fmts[(int)(*pdrvr++)], (INT_OR_PTR)(*(ppc)), 
				 (INT_OR_PTR)(*(ppc+1)));
#else
		j = fnprintf(fp, size, fmts[(int)(*pdrvr++)], (INT_OR_PTR)(*(ppc)), 
					  (INT_OR_PTR)(*(ppc+1)));
		assert(j == *ppc);
#endif
		if (size > *ppc) {
			size -= *ppc;
		}
		cnt += *ppc;			/* to avoid problems if j == -1 */
		ppc += 2;
	}

	return cnt;
}






