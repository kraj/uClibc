/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define __Wstrcmp __wcscmp
# define Wstrcmp wcscmp
#else
# define __Wstrcmp __strcmp
# define Wstrcmp strcmp
#endif

int attribute_hidden __Wstrcmp(register const Wchar *s1, register const Wchar *s2)
{
#ifdef WANT_WIDE
	while (*((Wuchar *)s1) == *((Wuchar *)s2)) {
		if (!*s1++) {
			return 0;
		}
		++s2;
	}

	return (*((Wuchar *)s1) < *((Wuchar *)s2)) ? -1 : 1;
#else
	int r;

	while (((r = ((int)(*((Wuchar *)s1))) - *((Wuchar *)s2++))
			== 0) && *s1++);
	
	return r;
#endif
}

strong_alias(__Wstrcmp,Wstrcmp)

#ifndef __UCLIBC_HAS_LOCALE__
# ifdef WANT_WIDE
hidden_strong_alias(__wcscmp,__wcscoll)
strong_alias(__wcscmp,wcscoll)
# else
hidden_strong_alias(__strcmp,__strcoll)
strong_alias(__strcmp,strcoll)
# endif
#endif
