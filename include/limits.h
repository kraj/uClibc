/* Copyright (C) 1991, 1992 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

/*
 *	ANSI Standard: 4.14/2.2.4.2 Limits of integral types	<limits.h>
 */

#ifndef	_LIMITS_H

#define	_LIMITS_H	1
#include <features.h>

#ifdef	__USE_POSIX
/* POSIX adds things to <limits.h>.  */
#include <posix1_lim.h>
#endif

#ifdef	__USE_POSIX2
#include <posix2_lim.h>
#endif


/* Only if gcc 2.x is used and -traditional is not used, we can use
 * #include_next.
#if	__GNUC__ >= 2 && __STDC__
 */
#if	__GNUC__ >= 2

 /* Have we done that? */
# if !defined(_GCC_LIMITS_H_) && !defined(_GCC_LIMITS_H)
  /* Get the compiler's limits.h, which defines all the ANSI
   * constants.
   */
  /* This tells it not to look for another.  */
# define _LIBC_LIMITS_H
# define _LIBC_LIMITS_H_
# include_next <limits.h>
# endif

#else	/* Not GCC 2.  */

/* We don't have #include_next.
   Define ANSI <limits.h> for standard 32-bit words.  */

/* These assume 8-bit `char's, 16-bit `short int's,
   and 32-bit `int's and `long int's.  */

/* Number of bits in a `char'.	*/
#define	CHAR_BIT	8

/* Maximum length of any multibyte character in any locale.
   Locale-writers should change this as necessary.  */
#define	MB_LEN_MAX	1

/* Minimum and maximum values a `signed char' can hold.  */
#define	SCHAR_MIN	(-128)
#define	SCHAR_MAX	127

/* Maximum value an `unsigned char' can hold.  (Minimum is 0.)  */
#ifdef	__STDC__
#define	UCHAR_MAX	255U
#else
#define	UCHAR_MAX	255
#endif

/* Minimum and maximum values a `char' can hold.  */
#ifdef __CHAR_UNSIGNED__
#define	CHAR_MIN	0
#define	CHAR_MAX	UCHAR_MAX
#else
#define	CHAR_MIN	SCHAR_MIN
#define	CHAR_MAX	SCHAR_MAX
#endif

/* Minimum and maximum values a `signed short int' can hold.  */
#define	SHRT_MIN	(-32768)
#define	SHRT_MAX	32767

/* Maximum value an `unsigned short int' can hold.  (Minimum is 0.)  */
#define	USHRT_MAX	65535

/* Minimum and maximum values a `signed int' can hold.  */
#define	INT_MIN	(- INT_MAX - 1)
#define	INT_MAX	2147483647

/* Maximum value an `unsigned int' can hold.  (Minimum is 0.)  */
#ifdef	__STDC__
#define	UINT_MAX	4294967295U
#else
#define	UINT_MAX	4294967295
#endif

/* Minimum and maximum values a `signed long int' can hold.  */
#define	LONG_MIN	INT_MIN
#define	LONG_MAX	INT_MAX

/* Maximum value an `unsigned long int' can hold.  (Minimum is 0.)  */
#define	ULONG_MAX	UINT_MAX

#endif	/* GCC 2.  */


#ifndef RAND_MAX
/* The largest number rand will return (same as INT_MAX).  */
#define RAND_MAX	INT_MAX
#endif

#ifdef __SVR4_I386_ABI_L1__

#define NL_ARGMAX	9
#define NL_LANGMAX	14
#define NL_MSGMAX	32767
#define NL_NMAX		1
#define NL_SETMAX	255
#define NL_TEXTMAX	255
#define NZERO		20

#define WORD_BIT	32
#define LONG_BIT	32

#define FCHR_MAX	1048576

#endif /* __SVR4_I386_ABI_L1__ */

#endif	/* limits.h  */
