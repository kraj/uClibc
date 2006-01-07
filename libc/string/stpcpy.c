/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define __Wstpcpy __wcpcpy
# define Wstpcpy wcpcpy
#else
# define __Wstpcpy __stpcpy
# define Wstpcpy stpcpy
#endif

Wchar attribute_hidden *__Wstpcpy(register Wchar * __restrict s1, const Wchar * __restrict s2)
{
#ifdef __BCC__
	do {
		*s1 = *s2++;
	} while (*s1++ != 0);
#else
	while ( (*s1++ = *s2++) != 0 );
#endif

	return s1 - 1;
}

strong_alias(__Wstpcpy,Wstpcpy)
