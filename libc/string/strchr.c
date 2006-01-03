/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define __Wstrchr __wcschr
# define Wstrchr wcschr
#else
# define __Wstrchr __strchr
# define Wstrchr strchr
#endif

Wchar attribute_hidden *__Wstrchr(register const Wchar *s, Wint c)
{
	do {
		if (*s == ((Wchar)c)) {
			return (Wchar *) s;	/* silence the warning */
		}
	} while (*s++);

	return NULL;
}

strong_alias(__strchr,strchr)

#ifndef WANT_WIDE
strong_alias(__strchr,index)
#endif
