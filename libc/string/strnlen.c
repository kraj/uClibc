/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define __Wstrnlen __wcsnlen
# define Wstrnlen wcsnlen
#else
# define __Wstrnlen __strnlen
# define Wstrnlen strnlen
#endif

size_t attribute_hidden __Wstrnlen(const Wchar *s, size_t max)
{
	register const Wchar *p = s;
#ifdef __BCC__
	/* bcc can optimize the counter if it thinks it is a pointer... */
	register const char *maxp = (const char *) max;
#else
# define maxp max
#endif

	while (maxp && *p) {
		++p;
		--maxp;
	}

	return p - s;
}
#undef maxp

strong_alias(__Wstrnlen,Wstrnlen)
