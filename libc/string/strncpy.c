/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define __Wstrncpy __wcsncpy
# define Wstrncpy wcsncpy
#else
# define __Wstrncpy __strncpy
# define Wstrncpy strncpy
#endif

Wchar attribute_hidden *__Wstrncpy(Wchar * __restrict s1, register const Wchar * __restrict s2,
				size_t n)
{
	register Wchar *s = s1;

#ifdef __BCC__
	while (n--) {
		if ((*s = *s2) != 0) s2++; /* Need to fill tail with 0s. */
		++s;
	}
#else
	while (n) {
		if ((*s = *s2) != 0) s2++; /* Need to fill tail with 0s. */
		++s;
		--n;
	}
#endif
	
	return s1;
}

strong_alias(__Wstrncpy,Wstrncpy)
