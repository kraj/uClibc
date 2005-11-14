/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strpbrk
#define WANT_WIDE
#define Wstrpbrk __wcspbrk

#include "wstring.c"

strong_alias(__wcspbrk, wcspbrk)

#undef L_strpbrk
