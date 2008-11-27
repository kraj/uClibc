/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "_string.h"

#ifdef WANT_WIDE
# define Wstrlcpy __wcslcpy
#else
# define Wstrlcpy strlcpy
#endif

/* OpenBSD function:
 * Copy at most n-1 chars from src to dst and nul-terminate dst.
 * Returns strlen(src), so truncation occurred if the return value is >= n. */

#ifdef WANT_WIDE
size_t Wstrlcpy(register Wchar *__restrict dst,
				  register const Wchar *__restrict src,
				  size_t n) attribute_hidden;
#endif
size_t Wstrlcpy(register Wchar *__restrict dst,
				  register const Wchar *__restrict src,
				  size_t n)
{
	const Wchar *src0 = src;
	Wchar dummy[1];

	if (!n) {
		dst = dummy;
	} else {
		--n;
	}

	while ((*dst = *src) != 0) {
		if (n) {
			--n;
			++dst;
		}
		++src;
	}

	return src - src0;
}
#ifdef WANT_WIDE

#ifndef __UCLIBC_HAS_LOCALE__
strong_alias(__wcslcpy,wcsxfrm)
libc_hidden_def(wcsxfrm)
#endif

#else

libc_hidden_def(strlcpy)
#ifndef __UCLIBC_HAS_LOCALE__
strong_alias(strlcpy,strxfrm)
libc_hidden_def(strxfrm)
#endif

#endif
