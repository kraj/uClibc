/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define __Wstrrchr __wcsrchr
# define Wstrrchr wcsrchr
#else
# define __Wstrrchr __strrchr
# define Wstrrchr strrchr
#endif

Wchar attribute_hidden *__Wstrrchr(register const  Wchar *s, Wint c)
{
	register const Wchar *p;

	p = NULL;
	do {
		if (*s == (Wchar) c) {
			p = s;
		}
	} while (*s++);

	return (Wchar *) p;			/* silence the warning */
}

strong_alias(__Wstrrchr,Wstrrchr)
#ifndef WANT_WIDE
strong_alias(__strrchr,rindex)
#endif
