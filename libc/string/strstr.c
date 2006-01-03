/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define __Wstrstr __wcsstr
# define Wstrstr wcsstr
#else
# define __Wstrstr __strstr
# define Wstrstr strstr
#endif

/* NOTE: This is the simple-minded O(len(s1) * len(s2)) worst-case approach. */

Wchar attribute_hidden *__Wstrstr(const Wchar *s1, const Wchar *s2)
{
	register const Wchar *s = s1;
	register const Wchar *p = s2;

	do {
		if (!*p) {
			return (Wchar *) s1;;
		}
		if (*p == *s) {
			++p;
			++s;
		} else {
			p = s2;
			if (!*s) {
				return NULL;
			}
			s = ++s1;
		}
	} while (1);
}

strong_alias(__Wstrstr,Wstrstr)
#ifdef WANT_WIDE
strong_alias(__wcsstr,wcswcs)
#endif
