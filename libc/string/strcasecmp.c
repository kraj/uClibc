/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"
#include <ctype.h>
#include <locale.h>

#ifdef __UCLIBC_HAS_XLOCALE__
extern int __strcasecmp_l (__const char *__s1, __const char *__s2,
			 __locale_t __loc)
     __THROW __attribute_pure__ __nonnull ((1, 2, 3)) attribute_hidden;
extern int __wcscasecmp_l (__const wchar_t *__s1, __const wchar_t *__s2,
			 __locale_t __loc) __THROW attribute_hidden;
#endif

#ifdef WANT_WIDE
# define strcasecmp wcscasecmp
# define __strcasecmp __wcscasecmp
# define strcasecmp_l wcscasecmp_l
# define __strcasecmp_l __wcscasecmp_l
# ifdef __UCLIBC_DO_XLOCALE
#  define TOLOWER(C) __towlower_l((C), locale_arg)
extern wint_t __towlower_l (wint_t __wc, __locale_t __locale) __THROW attribute_hidden;
# else
#  define TOLOWER(C) __towlower((C))
# endif
#else
# ifdef __UCLIBC_DO_XLOCALE
#  define TOLOWER(C) __tolower_l((C), locale_arg)
# else
#  define TOLOWER(C) tolower((C))
# endif
#endif

#if defined(__UCLIBC_HAS_XLOCALE__) && !defined(__UCLIBC_DO_XLOCALE)

int attribute_hidden __strcasecmp(register const Wchar *s1, register const Wchar *s2)
{
	return __strcasecmp_l(s1, s2, __UCLIBC_CURLOCALE);
}
strong_alias(__strcasecmp,strcasecmp)

#else  /* defined(__UCLIBC_HAS_XLOCALE__) && !defined(__UCLIBC_DO_XLOCALE) */

int attribute_hidden __UCXL(strcasecmp)(register const Wchar *s1, register const Wchar *s2
					  __LOCALE_PARAM )
{
#ifdef WANT_WIDE
	while ((*s1 == *s2) || (TOLOWER(*s1) == TOLOWER(*s2))) {
		if (!*s1++) {
			return 0;
		}
		++s2;
	}

	return (((Wuchar)TOLOWER(*s1)) < ((Wuchar)TOLOWER(*s2))) ? -1 : 1;
	/* TODO -- should wide cmp funcs do wchar or Wuchar compares? */
#else
	int r = 0;

	while ( ((s1 == s2) ||
			 !(r = ((int)( TOLOWER(*((Wuchar *)s1))))
			   - TOLOWER(*((Wuchar *)s2))))
			&& (++s2, *s1++));

	return r;
#endif
}
__UCXL_ALIAS(strcasecmp)

#endif /* defined(__UCLIBC_HAS_XLOCALE__) && !defined(__UCLIBC_DO_XLOCALE) */
