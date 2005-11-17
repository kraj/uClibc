/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strcmp
#define WANT_WIDE
#define Wstrcmp __wcscmp

#include "wstring.c"

strong_alias(__wcscmp, wcscmp)

#ifdef __LOCALE_C_ONLY
weak_alias(__wcscmp, __wcscoll)
strong_alias(__wcscoll, wcscoll)
#endif

#undef L_strcmp
