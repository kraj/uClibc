/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"
#include <stdlib.h>

#ifdef WANT_WIDE
libc_hidden_proto(wcslen)
libc_hidden_proto(wcscpy)
# define Wstrdup wcsdup
# define Wstrlen wcslen
# define Wstrcpy wcscpy
#else
libc_hidden_proto(strdup)
libc_hidden_proto(strlen)
libc_hidden_proto(strcpy)
# define Wstrdup strdup
# define Wstrlen strlen
# define Wstrcpy strcpy
#endif

Wchar *Wstrdup(register const Wchar *s1)
{
	register Wchar *s;

    if ((s = malloc((Wstrlen(s1) + 1) * sizeof(Wchar))) != NULL) {
		Wstrcpy(s, s1);
	}

	return s;
}

#ifndef WANT_WIDE
libc_hidden_def(strdup)
#endif
