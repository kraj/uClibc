/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define __Wstpncpy __wcpncpy
# define Wstpncpy wcpncpy
#else
# define __Wstpncpy __stpncpy
# define Wstpncpy stpncpy
#endif

Wchar attribute_hidden *__Wstpncpy(register Wchar * __restrict s1,
				register const Wchar * __restrict s2,
				size_t n)
{
	Wchar *s = s1;
	const Wchar *p = s2;

#ifdef __BCC__
	while (n--) {
		if ((*s = *s2) != 0) s2++; /* Need to fill tail with 0s. */
		++s;
	}
	return s1 + (s2 - p);
#else
	while (n) {
		if ((*s = *s2) != 0) s2++; /* Need to fill tail with 0s. */
		++s;
		--n;
	}
	return s1 + (s2 - p);
#endif
}

strong_alias(__Wstpncpy,Wstpncpy)
