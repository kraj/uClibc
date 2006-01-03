/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"
#include <stdlib.h>

#ifdef WANT_WIDE
# define __Wstrdup __wcsdup
# define Wstrdup wcsdup
# define Wstrlen __wcslen
# define Wstrcpy __wcscpy
#else
# define __Wstrdup __strdup
# define Wstrdup strdup
# define Wstrlen __strlen
# define Wstrcpy __strcpy
#endif

Wchar attribute_hidden *__Wstrdup(register const Wchar *s1)
{
	register Wchar *s;

    if ((s = malloc((Wstrlen(s1) + 1) * sizeof(Wchar))) != NULL) {
		Wstrcpy(s, s1);
	}

	return s;
}

strong_alias(__Wstrdup,Wstrdup)
