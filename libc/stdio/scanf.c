
/*
 * Modified by Manuel Novoa III       Mar 13, 2001
 *
 * The vfscanf routine was completely rewritten to add features and remove
 * bugs.  The function __strtold, based on my strtod code in stdlib, was
 * added to provide floating point support for the scanf functions.
 *
 * So far they pass the test cases from glibc-2.1.3, except in two instances.
 * In one case, the test appears to be broken.  The other case is something
 * I need to research further.  This version of scanf assumes it can only
 * peek one character ahead.  Apparently, glibc looks further.  The difference
 * can be seen when parsing a floating point value in the character
 * sequence "100ergs".  glibc is able to back up before the 'e' and return
 * a value of 100, whereas this scanf reports a bad match with the stream
 * pointer at 'r'.  A similar situation can also happen when parsing hex
 * values prefixed by 0x or 0X; a failure would occur for "0xg".  In order to
 * fix this, I need to rework the "ungetc" machinery in stdio.c again.
 * I do have one reference though, that seems to imply scanf has a single
 * character of lookahead.
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#ifdef L_scanf
#ifdef __STDC__
int scanf(const char *fmt, ...)
#else
int scanf(fmt, va_alist)
__const char *fmt;
va_dcl
#endif
{
	va_list ptr;
	int rv;

	va_start(ptr, fmt);
	rv = vfscanf(stdin, fmt, ptr);
	va_end(ptr);
	return rv;
}
#endif

#ifdef L_sscanf
#ifdef __STDC__
int sscanf(const char *sp, const char *fmt, ...)
#else
int sscanf(sp, fmt, va_alist)
__const char *sp;
__const char *fmt;
va_dcl
#endif
{
	FILE string[1] = {
		{0, (unsigned char *) ((unsigned) -1), 0, 0, (char *) ((unsigned) -1),
		 0, -1, _IOFBF}
	};

	va_list ptr;
	int rv;

	string->bufpos = (unsigned char *) ((void *) sp);
	va_start(ptr, fmt);
	rv = vfscanf(string, fmt, ptr);
	va_end(ptr);
	return rv;
}
#endif

#ifdef L_fscanf
#ifdef __STDC__
int fscanf(FILE * fp, const char *fmt, ...)
#else
int fscanf(fp, fmt, va_alist)
FILE *fp;
__const char *fmt;
va_dcl
#endif
{
	va_list ptr;
	int rv;

	va_start(ptr, fmt);
	rv = vfscanf(fp, fmt, ptr);
	va_end(ptr);
	return rv;
}
#endif

#ifdef L_vscanf
int vscanf(fmt, ap)
__const char *fmt;
va_list ap;
{
	return vfscanf(stdin, fmt, ap);
}
#endif

#ifdef L_vsscanf
int vsscanf(__const char *sp, __const char *fmt, va_list ap)
{
	FILE string[1] = {
		{0, (unsigned char *) ((unsigned) -1), 0, 0, (char *) ((unsigned) -1),
		 0, -1, _IOFBF}
	};

	string->bufpos = (unsigned char *) sp;
	return vfscanf(string, fmt, ap);
}
#endif

#ifdef L_vfscanf

#include <assert.h>
#include <ctype.h>
#include <limits.h>

static int valid_digit(char c, char base)
{
	if (base == 16) {
		return isxdigit(c);
	} else {
		return (isdigit(c) && (c < '0' + base));
	}
}

extern unsigned long long
_strto_ll(const char *str, char **endptr, int base, int uflag);

extern unsigned long
_strto_l(const char *str, char **endptr, int base, int uflag);

/* #define	skip()	do{c=getc(fp); if (c<1) goto done;}while(isspace(c))*/

#if WANT_LONG_LONG || WANT_LONG_LONG_ERROR
static const char qual[] = "hl" /* "jtz" */ "Lq";
/* char = -2, short = -1, int = 0, long = 1, long long = 2 */
static const char qsz[] = { -1, 1,           2, 2 };
#else
static const char qual[] = "hl" /* "jtz" */;
static const char qsz[] = { -1, 1,         };
#endif

#if WANT_DOUBLE || WANT_DOUBLE_ERROR
						   /*01234567890123456 */
static const char spec[]  = "%n[csoupxXidfeEgG";
#else
static const char spec[]  = "%n[csoupxXid";
#endif
/* radix[i] <-> spec[i+5]     o   u   p   x   X  i   d */
static const char radix[] = { 8, 10, 16, 16, 16, 0, 10 };

struct scan_cookie {
	FILE *fp;
	int nread;
	int width;
	int ungot_char;
	int ungot_flag;
};

static int __strtold(long double *ld, struct scan_cookie *sc);

static void init_scan_cookie(struct scan_cookie *sc, FILE *fp)
{
	sc->fp = fp;
	sc->nread = 0;
	sc->ungot_flag = 0;
	if ((sc->ungot_char = getc(fp)) > 0) { /* not EOF or EOS */
		sc->ungot_flag = 1;
	}
}

static int scan_getc_nw(struct scan_cookie *sc)
{
	if (sc->ungot_flag == 0) {
		sc->ungot_char = getc(sc->fp);
	} else {
		sc->ungot_flag = 0;
	}
	if (sc->ungot_char > 0) {
		++sc->nread;
	}
	return sc->ungot_char;
}

static int scan_getc(struct scan_cookie *sc)
{
	if (sc->ungot_flag == 0) {
		sc->ungot_char = getc(sc->fp);
	}
	if (--sc->width < 0) {
		sc->ungot_flag = 1;
		return 0;
	}
	sc->ungot_flag = 0;
	if (sc->ungot_char > 0) {
		++sc->nread;
	}
	return sc->ungot_char;
}

static void scan_ungetc(struct scan_cookie *sc)
{
	if (sc->ungot_flag != 0) {
		assert(sc->width < 0);
		return;
	}
	sc->ungot_flag = 1;
	if (sc->ungot_char > 0) {	/* not EOF or EOS */
		--sc->nread;
	}
}

static void kill_scan_cookie(struct scan_cookie *sc)
{
	if (sc->ungot_flag) {
		ungetc(sc->ungot_char,sc->fp);
	}
}

int vfscanf(fp, format, ap)
FILE *fp;
const char *format;
va_list ap;
{
#if WANT_LONG_LONG
#define STRTO_L_(s,e,b,u) _strto_ll(s,e,b,u)
#define MAX_DIGITS 64
#define UV_TYPE unsigned long long
#define V_TYPE long long
#else
#define STRTO_L_(s,e,b,u) _strto_l(s,e,b,u)
#define MAX_DIGITS 32
#define UV_TYPE unsigned long
#define V_TYPE long
#endif
#if WANT_DOUBLE
	long double ld;
#endif
	UV_TYPE uv;
	struct scan_cookie sc;
	unsigned const char *fmt;
	const char *p;
	unsigned char *b;
	void *vp;
	int cc, i, cnt;
	signed char lval;
	unsigned char store, usflag, base, invert, r0, r1;
	unsigned char buf[MAX_DIGITS+2];
	unsigned char scanset[UCHAR_MAX + 1];

	init_scan_cookie(&sc,fp);

	fmt = (unsigned const char *) format;
	cnt = 0;

	while (*fmt) {
		store = 1;
		lval = 0;
		sc.width = INT_MAX;
		if (*fmt == '%') {		/* Conversion specification. */
			++fmt;
			if (*fmt == '*') {	/* Suppress assignment. */
				store = 0;
				++fmt;
			}
			for (i = 0 ; isdigit(*fmt) ; sc.width = i) {
				i = (i * 10) + (*fmt++ - '0'); /* Get specified width. */
			}
			for (i = 0 ; i < sizeof(qual) ; i++) { /* Optional qualifier. */
				if (qual[i] == *fmt) {
					++fmt;
					lval += qsz[i];
					if ((i < 2) && (qual[i] == *fmt)) {	/* Double h or l. */
						++fmt;
						lval += qsz[i];
					}
					break;
				}
			}
			for (p = spec ; *p ; p++) {	/* Process format specifier. */
				if (*fmt != *p) continue;
				if (p-spec < 1) { /* % - match a '%'*/
					goto matchchar;
				}
				if (p-spec < 2) { /* n - store number of chars read */
					*(va_arg(ap, int *)) = sc.nread;
					scan_getc_nw(&sc);
					goto nextfmt;
				}
				if (p-spec > 3) { /* skip white space if not c or [ */
					while (isspace(scan_getc_nw(&sc)))
						{}
					scan_ungetc(&sc);
				}
				if (p-spec < 5) { /* [,c,s - string conversions */
					if ((*p == 'c') && (sc.width == INT_MAX)) {
						sc.width = 1;
					}
					invert = 0;
					for (i=0 ; i<= UCHAR_MAX ; i++) {
						scanset[i] = ((*p == 's') ? (isspace(i) == 0) : 0);
					}
					if (*p == '[') { /* need to build a scanset */
						if (*++fmt == '^') {
							invert = 1;
							++fmt;
						}
						if (*fmt == ']') {
							scanset[(int)']'] = 1;
							++fmt;
						}
						r0 = 0;
						while (*fmt && *fmt !=']') { /* build scanset */
							if ((*fmt == '-') && r0 && (fmt[1] != ']')) {
								/* range */
								++fmt;
								if (*fmt < r0) {
									r1 = r0;
									r0 = *fmt;
								} else {
									r1 = *fmt;
								}
								for (i=r0 ; i<= r1 ; i++) {
									scanset[i] = 1;
								}
								r0 = 0;
							} else {
								r0 = *fmt;
								scanset[r0] = 1;
							}
							++fmt;
						}
						if (!*fmt) { /* format string exhausted! */
							goto done;
						}
					}
					/* ok -- back to common work */
					if (sc.width <= 0) {
						goto done;
					}
					if (store) {
						b = va_arg(ap, unsigned char *);
					} else {
						b = buf;
					}
					i = 0;
					cc = scan_getc(&sc);
					while ((cc>0) && (scanset[cc] != invert)) {
						i = store; /* yes, we stored something */
						*b = cc;
						b += store;
						cc = scan_getc(&sc);
					}
					if (*p != 'c') { /* nul-terminate the stored string */
						*b = 0;
						cnt += i;
						goto nextfmt;
					} else if (sc.width < 0) { /* case 'c' */
						cnt += store;
						goto nextfmt;
					}
					scan_ungetc(&sc);
					goto done;
				}
				if (p-spec < 12) { /* o,u,p,x,X,i,d - (un)signed integer */
					if (*p == 'p') {
						/* assume pointer same size as int or long. */
						lval = (sizeof(char *) == sizeof(long));
					}
					usflag = ((p-spec) < 10); /* (1)0 if (un)signed */
					base = radix[(int)(p-spec) - 5];
					b = buf;
					if (sc.width <= 0) {
						goto done;
					}
					cc = scan_getc(&sc);
					if ((cc == '+') || (cc == '-')) { /* Handle leading sign.*/
						*b++ = cc;
						cc = scan_getc(&sc);
					}
					if (cc == '0') { /* Possibly set base and handle prefix. */
						if ((base == 0) || (base == 16)) {
							cc = scan_getc(&sc);
							if ((cc == 'x') || (cc == 'X')) {
								/* We're committed to base 16 now. */
								base = 16;
								cc = scan_getc(&sc);
							} else { /* oops... back up */
								scan_ungetc(&sc);
								cc = '0';
								if (base == 0) {
									base = 8;
								}
							}
						}
					}
					/* At this point, we're ready to start reading digits. */
					if (cc == '0') {
						*b++ = cc; /* Store first leading 0 */
						do {	/*     but ignore others. */
							cc = scan_getc(&sc);
						} while (cc == '0');
					}
					while (valid_digit(cc,base)) { /* Now for nonzero digits.*/
						if (b - buf < MAX_DIGITS) {
							*b++ = cc;
						}
						cc = scan_getc(&sc);
					}
					*b = 0;	/* null-terminate */
					if ((b == buf) || (*--b == '+') || (*b == '-')) {
						scan_ungetc(&sc);
						goto done; /* No digits! */
					}
					if (store) {
						if (*buf == '-') {
							usflag = 0;
						}
						uv = STRTO_L_(buf, NULL, base, usflag);
						vp = va_arg(ap, void *);
						switch (lval) {
							case 2:	/* If no long long, treat as long . */
#if WANT_LONG_LONG
								*((unsigned long long *)vp) = uv;
								break;
#endif
							case 1:
#if ULONG_MAX == UINT_MAX
							case 0:	/* int and long int are the same */
#endif
#if WANT_LONG_LONG
								if (usflag) {
									if (uv > ULONG_MAX) {
										uv = ULONG_MAX;
									}
								} else if (((V_TYPE)uv) > LONG_MAX) {
									uv = LONG_MAX;
								} else if (((V_TYPE)uv) < LONG_MIN) {
									uv = (UV_TYPE) LONG_MIN;
								}
#endif
								*((unsigned long *)vp) = (unsigned long)uv;
								break;
#if ULONG_MAX != UINT_MAX
							case 0:	/* int and long int are different */
								if (usflag) {
									if (uv > UINT_MAX) {
										uv = UINT_MAX;
									}
								} else if (((V_TYPE)uv) > INT_MAX) {
									uv = INT_MAX;
								} else if (((V_TYPE)uv) < INT_MIN) {
									uv = (UV_TYPE) INT_MIN;
								}
								*((unsigned int *)vp) = (unsigned int)uv;
								break;
#endif
							case -1:
								if (usflag) {
									if (uv > USHRT_MAX) {
										uv = USHRT_MAX;
									}
								} else if (((V_TYPE)uv) > SHRT_MAX) {
									uv = SHRT_MAX;
								} else if (((V_TYPE)uv) < SHRT_MIN) {
									uv = (UV_TYPE) SHRT_MIN;
								}
								*((unsigned short *)vp) = (unsigned short)uv;
								break;
							case -2:
								if (usflag) {
									if (uv > UCHAR_MAX) {
										uv = UCHAR_MAX;
									}
								} else if (((V_TYPE)uv) > CHAR_MAX) {
									uv = CHAR_MAX;
								} else if (((V_TYPE)uv) < CHAR_MIN) {
									uv = (UV_TYPE) CHAR_MIN;
								}
								*((unsigned char *)vp) = (unsigned char) uv;
								break;
							default:
								assert(0);
						}
						++cnt;
					}
					goto nextfmt;
				}
#if WANT_DOUBLE
				else {			/* floating point */
					if (sc.width <= 0) {
						goto done;
					}
					if (__strtold(&ld, &sc)) { /* Success! */
						if (store) {
							vp = va_arg(ap, void *);
							switch (lval) {
								case 2:
									*((long double *)vp) = ld;
									break;
								case 1:
									*((double *)vp) = (double) ld;
									break;
								case 0:
									*((float *)vp) = (float) ld;
									break;
								default: /* Illegal qualifier! */
									assert(0);
									goto done;
							}
							++cnt;
						}
						goto nextfmt;
					}
				}
#else
				assert(0);
#endif
				goto done;
			}
			/* Unrecognized specifier! */
			goto done;
		} if (isspace(*fmt)) {	/* Consume all whitespace. */
			while (isspace(scan_getc_nw(&sc)))
				{}
		} else {				/* Match the current fmt char. */
		matchchar:
			if (scan_getc_nw(&sc) != *fmt) {
				goto done;
			}
			scan_getc_nw(&sc);
		}
	nextfmt:
		scan_ungetc(&sc);
		++fmt;
	}

  done:						/* end of scan */
	kill_scan_cookie(&sc);

	if ((sc.ungot_char <= 0) && (cnt == 0) && (*fmt)) {
		return (EOF);
	}

	return (cnt);
}

/*****************************************************************************/
#if WANT_DOUBLE

#include <float.h>

#define MAX_SIG_DIGITS 20
#define MAX_IGNORED_DIGITS 2000
#define MAX_ALLOWED_EXP (MAX_SIG_DIGITS + MAX_IGNORED_DIGITS + LDBL_MAX_10_EXP)

#if LDBL_DIG > MAX_SIG_DIGITS
#error need to adjust MAX_SIG_DIGITS
#endif

#include <limits.h>
#if MAX_ALLOWED_EXP > INT_MAX
#error size assumption violated for MAX_ALLOWED_EXP
#endif

int __strtold(long double *ld, struct scan_cookie *sc)
{
    long double number;
    long double p10;
    int exponent_power;
    int exponent_temp;
    int negative;
    int num_digits;
    int since_decimal;
	int c;

	c = scan_getc(sc);				/* Decrements width. */

    negative = 0;
    switch(c) {					/* Handle optional sign. */
		case '-': negative = 1;	/* Fall through to get next char. */
		case '+': c = scan_getc(sc);
    }

    number = 0.;
    num_digits = -1;
    exponent_power = 0;
    since_decimal = INT_MIN;

 LOOP:
    while (isdigit(c)) {		/* Process string of digits. */
		++since_decimal;
		if (num_digits < 0) {	/* First time through? */
			++num_digits;		/* We've now seen a digit. */
		}
		if (num_digits || (c != '0')) { /* had/have nonzero */
			++num_digits;
			if (num_digits <= MAX_SIG_DIGITS) { /* Is digit significant? */
				number = number * 10. + (c - '0');
			}
		}
		c = scan_getc(sc);
    }

    if ((c == '.') && (since_decimal < 0)) { /* If no previous decimal pt, */
		since_decimal = 0;		/* save position of decimal point */
		c = scan_getc(sc);			/* and process rest of digits */
		goto LOOP;
    }

    if (num_digits<0) {			/* Must have at least one digit. */
		goto FAIL;
    }

    if (num_digits > MAX_SIG_DIGITS) { /* Adjust exp for skipped digits. */
		exponent_power += num_digits - MAX_SIG_DIGITS;
    }

    if (since_decimal >= 0) {		/* Adjust exponent for decimal point. */
		exponent_power -= since_decimal;
    }

    if (negative) {				/* Correct for sign. */
		number = -number;
		negative = 0;			/* Reset for exponent processing below. */
    }

    /* Process an exponent string. */
    if (c == 'e' || c == 'E') {
		c = scan_getc(sc);
		switch(c) {				/* Handle optional sign. */
			case '-': negative = 1;	/* Fall through to get next char. */
			case '+': c = scan_getc(sc);
		}

		num_digits = 0;
		exponent_temp = 0;
		while (isdigit(c)) {	/* Process string of digits. */
			if (exponent_temp < MAX_ALLOWED_EXP) { /* overflow check */
				exponent_temp = exponent_temp * 10 + (c - '0');
			}
			c = scan_getc(sc);
			++num_digits;
		}

		if (num_digits == 0) {	/* Were there no exp digits? */
			goto FAIL;
		} /* else */
		if (negative) {
			exponent_power -= exponent_temp;
		} else {
			exponent_power += exponent_temp;
		}
    }

    if (number != 0.) {
		/* Now scale the result. */
		exponent_temp = exponent_power;
		p10 = 10.;

		if (exponent_temp < 0) {
			exponent_temp = -exponent_temp;
		}

		while (exponent_temp) {
			if (exponent_temp & 1) {
				if (exponent_power < 0) {
					number /= p10;
				} else {
					number *= p10;
				}
			}
			exponent_temp >>= 1;
			p10 *= p10;
		}
	}
	*ld = number;
	return 1;

 FAIL:
	scan_ungetc(sc);
	return 0;
}
#endif /* WANT_DOUBLE */
#endif
