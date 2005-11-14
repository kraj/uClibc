/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strlcpy
#define WANT_WIDE
#define Wstrlcpy __wcslcpy

#include "wstring.c"

#ifdef __LOCALE_C_ONLY
weak_alias(__wcslcpy, wcsxfrm)
#endif

#undef L_strlcpy
