/*
 * Copyright (C) 2000 Manuel Novoa III
 *
 * Notes:
 *
 * The primary objective of this implementation was minimal size.
 *
 * Note: Assumes char layout 0-9.*A-Z.*a-z for ordinals values.
 *
 * There are a couple of compile-time options below.
 *
 */

/*****************************************************************************/
/*                            OPTIONS                                        */
/*****************************************************************************/

/* Set if we want errno set appropriately. */
/* NOTE: Implies _STRTO_ENDPTR below */
#define _STRTO_ERRNO            0

/* Set if we want support for the endptr arg. */
/* Implied by _STRTO_ERRNO. */
#define _STRTO_ENDPTR           1

/*****************************************************************************/
/* Don't change anything that follows.                                       */
/*****************************************************************************/

#if _STRTO_ERRNO
#undef _STRTO_ENDPTR
#define _STRTO_ENDPTR           1
#endif

/*****************************************************************************/

/* Are there actually any machines where this might fail? */
#if 'A' > 'a'
#error ordering assumption violated : 'A' > 'a'
#endif

#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#if _STRTO_ERRNO
#include <errno.h>
#endif

unsigned long long _strto_ll(const char *str, char **endptr, int base, int uflag);

#if L_strto_ll

/*
 * This is the main work fuction which handles both strtol (uflag = 0) and
 * strtoul (uflag = 1).
 */

unsigned long long _strto_ll(const char *str, char **endptr, int base, int uflag)
{
    unsigned long long number = 0;
    unsigned long long cutoff;
    char *pos = (char *) str;
#if _STRTO_ENDPTR
    char *fail_char = (char *) str;
#endif
    int digit, cutoff_digit;
    int negative;

    while (isspace(*pos)) {	/* skip leading whitespace */
	++pos;
    }

    /* handle optional sign */
    negative = 0;
    switch(*pos) {
    case '-': negative = 1;	/* fall through to increment pos */
    case '+': ++pos;
    }

    if ((base == 16) && (*pos == '0')) { /* handle option prefix */
	++pos;
#if _STRTO_ENDPTR
	fail_char = pos;
#endif
	if ((*pos == 'x') || (*pos == 'X')) {
	    ++pos;
	}
    }
    
    if (base == 0) {		/* dynamic base */
	base = 10;		/* default is 10 */
	if (*pos == '0') {
	    ++pos;
	    base -= 2;		/* now base is 8 (or 16) */
#if _STRTO_ENDPTR
	    fail_char = pos;
#endif
	    if ((*pos == 'x') || (*pos == 'X')) {
		base += 8;	/* base is 16 */
		++pos;
	    }
	}
    }

    if ((base < 2) || (base > 36)) { /* illegal base */
	goto DONE;
    }

    cutoff_digit = ULONG_LONG_MAX % base;
    cutoff = ULONG_LONG_MAX / base;

    while (1) {
	digit = 40;
	if ((*pos >= '0') && (*pos <= '9')) {
	    digit = (*pos - '0');
	} else if (*pos >= 'a') {
	    digit = (*pos - 'a' + 10);
	} else if (*pos >= 'A') {
	    digit = (*pos - 'A' + 10);
	} else break;

	if (digit >= base) {
	    break;
	}

	++pos;
#if _STRTO_ENDPTR
	fail_char = pos;
#endif

	/* adjust number, with overflow check */
	if ((number > cutoff)
	    || ((number == cutoff) && (digit > cutoff_digit))) {
	    number = ULONG_LONG_MAX;
	    if (uflag) {
		negative = 0; /* since unsigned returns ULONG_LONG_MAX */
	    }
#if _STRTO_ERRNO
	    errno = ERANGE;
#endif
	} else {
	    number = number * base + digit;
	}

    }

 DONE:
#if _STRTO_ENDPTR
    if (endptr) {
	*endptr = fail_char;
    }
#endif

    if (negative) {
	if (!uflag && (number > ((unsigned long long)(-(1+LONG_LONG_MIN)))+1)) {
#if _STRTO_ERRNO
	    errno = ERANGE;
#endif
	    return (unsigned long long) LONG_LONG_MIN;
	}
	return (unsigned long long)(-((long long)number));
    } else {
	if (!uflag && (number > (unsigned long long) LONG_LONG_MAX)) {
#if _STRTO_ERRNO
	    errno = ERANGE;
#endif
	    return LONG_LONG_MAX;
	}
	return number;
    }
}

#endif

#if L_strtoull

unsigned long long strtoull(const char *str, char **endptr, int base)
{
    return _strto_ll(str, endptr, base, 1);
}

#endif

#if L_strtoll

long long strtoll(const char *str, char **endptr, int base)
{
    return _strto_ll(str, endptr, base, 0);
}

#endif
