/*  Copyright (C) 2002     Manuel Novoa III
 *  From my (incomplete) stdlib library for linux and (soon) elks.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*  ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION!
 *
 *  This code is currently under development.  Also, I plan to port
 *  it to elks which is a 16-bit environment with a fairly limited
 *  compiler.  Therefore, please refrain from modifying this code
 *  and, instead, pass any bug-fixes, etc. to me.  Thanks.  Manuel
 *
 *  ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION!   ATTENTION! */

#define _ISOC99_SOURCE			/* for ULLONG primarily... */
#define _GNU_SOURCE
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>

extern unsigned long
_stdlib_strto_l(register const char * __restrict str,
				char ** __restrict endptr, int base, int sflag);
extern unsigned long long
_stdlib_strto_ll(register const char * __restrict str,
				 char ** __restrict endptr, int base, int sflag);

/* TODO: gcc reports an error due to prototype conflicts.  Don't include
 * the header for the problem cases? */
#define HEADER_ALIAS_PROBLEM

/**********************************************************************/
#ifdef L_abs

#ifdef HEADER_ALIAS_PROBLEM
/*  #if UINT_MAX < ULONG_MAX */

int abs(int j)
{
	return (j >= 0) ? j : -j;
}

#endif /* UINT_MAX < ULONG_MAX */

#endif
/**********************************************************************/
#ifdef L_labs

#ifndef HEADER_ALIAS_PROBLEM
/*  #if UINT_MAX == ULONG_MAX */
strong_alias(labs,abs)
#endif

#if defined(ULLONG_MAX) && (ULLONG_MAX == ULONG_MAX)
strong_alias(labs,llabs)
#endif

#if ULONG_MAX == UINTMAX_MAX
strong_alias(labs,imaxabs)
#endif

long int labs(long int j)
{
	return (j >= 0) ? j : -j;
}

#endif
/**********************************************************************/
#ifdef L_llabs

#if defined(ULLONG_MAX) && (ULLONG_MAX > ULONG_MAX)

#if (ULLONG_MAX == UINTMAX_MAX)
strong_alias(llabs,imaxabs)
#endif

long long int llabs(long long int j)
{
	return (j >= 0) ? j : -j;
}

#endif /* defined(ULLONG_MAX) && (ULLONG_MAX > ULONG_MAX) */

#endif
/**********************************************************************/
#ifdef L_atoi

#ifdef HEADER_ALIAS_PROBLEM
/*  #if UINT_MAX < ULONG_MAX  */

int atoi(const char *nptr)
{
	return (int) strtol(nptr, (char **) NULL, 10);
}

#endif /* UINT_MAX < ULONG_MAX  */

#endif
/**********************************************************************/
#ifdef L_atol

#ifndef HEADER_ALIAS_PROBLEM
/*  #if UINT_MAX == ULONG_MAX */
strong_alias(atol,atoi)
#endif

#if defined(ULLONG_MAX) && (ULLONG_MAX == ULONG_MAX)
strong_alias(atol,atoll)
#endif

long atol(const char *nptr)
{
	return strtol(nptr, (char **) NULL, 10);
}

#endif
/**********************************************************************/
#ifdef L_atoll

#if defined(ULLONG_MAX) && (ULLONG_MAX > ULONG_MAX)

long long atoll(const char *nptr)
{
	return strtoll(nptr, (char **) NULL, 10);
}

#endif /* defined(ULLONG_MAX) && (ULLONG_MAX > ULONG_MAX) */

#endif
/**********************************************************************/
#ifdef L_strtol

#if ULONG_MAX == UINTMAX_MAX
strong_alias(strtol,strtoimax)
#endif

#if defined(ULLONG_MAX) && (ULLONG_MAX == ULONG_MAX)
strong_alias(strtol,strtoll)
#endif

long strtol(const char * __restrict str, char ** __restrict endptr, int base)
{
    return _stdlib_strto_l(str, endptr, base, 1);
}

#endif
/**********************************************************************/
#ifdef L_strtoll

#if defined(ULLONG_MAX) && (ULLONG_MAX > ULONG_MAX)

#if (ULLONG_MAX == UINTMAX_MAX)
strong_alias(strtoll,strtoimax)
#endif

long long strtoll(const char * __restrict str,
				  char ** __restrict endptr, int base)
{
    return (long long) _stdlib_strto_ll(str, endptr, base, 1);
}

#endif /* defined(ULLONG_MAX) && (ULLONG_MAX > ULONG_MAX) */

#endif
/**********************************************************************/
#ifdef L_strtoul

#if ULONG_MAX == UINTMAX_MAX
strong_alias(strtoul,strtoumax)
#endif

#if defined(ULLONG_MAX) && (ULLONG_MAX == ULONG_MAX)
strong_alias(strtoul,strtoull)
#endif

unsigned long strtoul(const char * __restrict str,
					  char ** __restrict endptr, int base)
{
    return _stdlib_strto_l(str, endptr, base, 0);
}

#endif
/**********************************************************************/
#ifdef L_strtoull

#if defined(ULLONG_MAX) && (ULLONG_MAX > ULONG_MAX)

#if (ULLONG_MAX == UINTMAX_MAX)
strong_alias(strtoull,strtoumax)
#endif

unsigned long long strtoull(const char * __restrict str,
							char ** __restrict endptr, int base)
{
    return _stdlib_strto_ll(str, endptr, base, 0);
}

#endif /* defined(ULLONG_MAX) && (ULLONG_MAX > ULONG_MAX) */

#endif
/**********************************************************************/
/* Support routines follow */
/**********************************************************************/
/* Set if we want errno set appropriately. */
/* NOTE: Implies _STRTO_ENDPTR below */
#define _STRTO_ERRNO            1

/* Set if we want support for the endptr arg. */
/* Implied by _STRTO_ERRNO. */
#define _STRTO_ENDPTR           1

#if _STRTO_ERRNO
#undef _STRTO_ENDPTR
#define _STRTO_ENDPTR           1
#define SET_ERRNO(X)            __set_errno(X)
#else
#define SET_ERRNO(X)            ((void)(X))	/* keep side effects */
#endif

/**********************************************************************/
#ifdef L__stdlib_strto_l

/* This is the main work fuction which handles both strtol (sflag = 1) and
 * strtoul (sflag = 0). */

unsigned long _stdlib_strto_l(register const char * __restrict str,
							  char ** __restrict endptr, int base, int sflag)
{
    unsigned long number, cutoff;
#if _STRTO_ENDPTR
    const char *fail_char;
#define SET_FAIL(X)       fail_char = (X)
#else
#define SET_FAIL(X)       ((void)(X)) /* Keep side effects. */
#endif
    unsigned char negative, digit, cutoff_digit;

	assert((sflag == 0) || (sflag == 1));

	SET_FAIL(str);

    while (isspace(*str)) {		/* Skip leading whitespace. */
		++str;
    }

    /* Handle optional sign. */
    negative = 0;
    switch(*str) {
		case '-': negative = 1;	/* Fall through to increment str. */
		case '+': ++str;
    }

    if (!(base & ~0x10)) {		/* Either dynamic (base = 0) or base 16. */
		base += 10;				/* Default is 10 (26). */
		if (*str == '0') {
			SET_FAIL(++str);
			base -= 2;			/* Now base is 8 or 16 (24). */
			if ((0x20|(*str)) == 'x') { /* WARNING: assumes ascii. */
				++str;
				base += base;	/* Base is 16 (16 or 48). */
			}
		}

		if (base > 16) {		/* Adjust in case base wasn't dynamic. */
			base = 16;
		}
    }

	number = 0;

    if (((unsigned)(base - 2)) < 35) { /* Legal base. */
		cutoff_digit = ULONG_MAX % base;
		cutoff = ULONG_MAX / base;
		do {
			digit = (((unsigned char)(*str - '0')) <= 9)
				? (*str - '0')
				: ((*str >= 'A')
				   ? (((0x20|(*str)) - 'a' + 10)) /* WARNING: assumes ascii. */
					  : 40);

			if (digit >= base) {
				break;
			}

			SET_FAIL(++str);

			if ((number > cutoff)
				|| ((number == cutoff) && (digit > cutoff_digit))) {
				number = ULONG_MAX;
				negative &= sflag;
				SET_ERRNO(ERANGE);
			} else {
				number = number * base + digit;
			}
		} while (1);
	}

#if _STRTO_ENDPTR
    if (endptr) {
		*endptr = (char *) fail_char;
    }
#endif

	{
		unsigned long tmp = ((negative)
							 ? ((unsigned long)(-(1+LONG_MIN)))+1
							 : LONG_MAX);
		if (sflag && (number > tmp)) {
			number = tmp;
			SET_ERRNO(ERANGE);
		}
	}

	return negative ? (unsigned long)(-((long)number)) : number;
}

#endif
/**********************************************************************/
#ifdef L__stdlib_strto_ll

#if defined(ULLONG_MAX) && (ULLONG_MAX > ULONG_MAX)

/* This is the main work fuction which handles both strtoll (sflag = 1) and
 * strtoull (sflag = 0). */

unsigned long long _stdlib_strto_ll(register const char * __restrict str,
									char ** __restrict endptr, int base,
									int sflag)
{
    unsigned long long number;
#if _STRTO_ENDPTR
    const char *fail_char;
#define SET_FAIL(X)       fail_char = (X)
#else
#define SET_FAIL(X)       ((void)(X)) /* Keep side effects. */
#endif
	unsigned int n1;
    unsigned char negative, digit;

	assert((sflag == 0) || (sflag == 1));

	SET_FAIL(str);

    while (isspace(*str)) {		/* Skip leading whitespace. */
		++str;
    }

    /* Handle optional sign. */
    negative = 0;
    switch(*str) {
		case '-': negative = 1;	/* Fall through to increment str. */
		case '+': ++str;
    }

    if (!(base & ~0x10)) {		/* Either dynamic (base = 0) or base 16. */
		base += 10;				/* Default is 10 (26). */
		if (*str == '0') {
			SET_FAIL(++str);
			base -= 2;			/* Now base is 8 or 16 (24). */
			if ((0x20|(*str)) == 'x') { /* WARNING: assumes ascii. */
				++str;
				base += base;	/* Base is 16 (16 or 48). */
			}
		}

		if (base > 16) {		/* Adjust in case base wasn't dynamic. */
			base = 16;
		}
    }

	number = 0;

    if (((unsigned)(base - 2)) < 35) { /* Legal base. */
		do {
			digit = (((unsigned char)(*str - '0')) <= 9)
				? (*str - '0')
				: ((*str >= 'A')
				   ? (((0x20|(*str)) - 'a' + 10)) /* WARNING: assumes ascii. */
					  : 40);

			if (digit >= base) {
				break;
			}

			SET_FAIL(++str);

#if 1
			/* Optional, but speeds things up in the usual case. */
			if (number <= (ULLONG_MAX >> 6)) {
				number = number * base + digit;
			} else
#endif
			{
				n1 = ((unsigned char) number) * base + digit;
				number = (number >> CHAR_BIT) * base;

				if (number + (n1 >> CHAR_BIT) <= (ULLONG_MAX >> CHAR_BIT)) {
					number = (number << CHAR_BIT) + n1;
				} else {		/* Overflow. */
					number = ULLONG_MAX;
					negative &= sflag;
					SET_ERRNO(ERANGE);
				}
			}

		} while (1);
	}

#if _STRTO_ENDPTR
    if (endptr) {
		*endptr = (char *) fail_char;
    }
#endif

	{
		unsigned long long tmp = ((negative)
								  ? ((unsigned long long)(-(1+LLONG_MIN)))+1
								  : LLONG_MAX);
		if (sflag && (number > tmp)) {
			number = tmp;
			SET_ERRNO(ERANGE);
		}
	}

	return negative ? (unsigned long long)(-((long long)number)) : number;
}

#endif /* defined(ULLONG_MAX) && (ULLONG_MAX > ULONG_MAX) */

#endif
/**********************************************************************/
