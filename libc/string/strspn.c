/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define __Wstrspn __wcsspn
# define Wstrspn wcsspn
#else
# define __Wstrspn __strspn
# define Wstrspn strspn
#endif

size_t attribute_hidden __Wstrspn(const Wchar *s1, const Wchar *s2)
{
	register const Wchar *s = s1;
	register const Wchar *p = s2;

	while (*p) {
		if (*p++ == *s) {
			++s;
			p = s2;
		}
	}
	return s - s1;
}

strong_alias(__Wstrspn,Wstrspn)
