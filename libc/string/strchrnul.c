/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define __Wstrchrnul __wcschrnul
# define Wstrchrnul wcschrnul
#else
# define __Wstrchrnul __strchrnul
# define Wstrchrnul strchrnul
#endif

Wchar attribute_hidden *__Wstrchrnul(register const Wchar *s, Wint c)
{
	--s;
	while (*++s && (*s != ((Wchar)c)));
	return (Wchar *) s;
}

strong_alias(__Wstrchrnul,Wstrchrnul)
