/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define __Wstrcat __wcscat
# define Wstrcat wcscat
#else
# define __Wstrcat __strcat
# define Wstrcat strcat
#endif

Wchar attribute_hidden *__Wstrcat(Wchar * __restrict s1, register const Wchar * __restrict s2)
{
	register Wchar *s = s1;

	while (*s++);
	--s;
	while ((*s++ = *s2++) != 0);

	return s1;
}

strong_alias(__Wstrcat,Wstrcat)
