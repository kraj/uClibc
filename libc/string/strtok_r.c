/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define __Wstrtok_r __wcstok
# define Wstrtok_r wcstok
# define Wstrspn __wcsspn
# define Wstrpbrk __wcspbrk
#else
# define __Wstrtok_r __strtok_r
# define Wstrtok_r strtok_r
# define Wstrspn __strspn
# define Wstrpbrk __strpbrk
#endif

Wchar attribute_hidden *__Wstrtok_r(Wchar * __restrict s1, const Wchar * __restrict s2,
				 Wchar ** __restrict next_start)
{
	register Wchar *s;
	register Wchar *p;

#if 1
	if (((s = s1) != NULL) || ((s = *next_start) != NULL)) {
		if (*(s += Wstrspn(s, s2))) {
			if ((p = Wstrpbrk(s, s2)) != NULL) {
				*p++ = 0;
			}
		} else {
			p = s = NULL;
		}
		*next_start = p;
	}
	return s;
#else
	if (!(s = s1)) {
		s = *next_start;
	}
	if (s && *(s += Wstrspn(s, s2))) {
		if (*(p = s + Wstrcspn(s, s2))) {
			*p++ = 0;
		}
		*next_start = p;
		return s;
	}
	return NULL;				/* TODO: set *next_start = NULL for safety? */
#endif
}

strong_alias(__Wstrtok_r,Wstrtok_r)
