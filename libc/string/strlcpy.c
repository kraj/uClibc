/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strlcpy
#define Wstrlcpy __strlcpy

#include "wstring.c"

strong_alias(__strlcpy, strlcpy)

#ifdef __LOCALE_C_ONLY
weak_alias(strlcpy, strxfrm)
#endif

#undef L_strlcpy
