/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define __Wstrlen __wcslen
# define Wstrlen wcslen
#else
# define __Wstrlen __strlen
# define Wstrlen strlen
#endif

size_t attribute_hidden __Wstrlen(const Wchar *s)
{
	register const Wchar *p;

	for (p=s ; *p ; p++);

	return p - s;
}

strong_alias(__Wstrlen,Wstrlen)
