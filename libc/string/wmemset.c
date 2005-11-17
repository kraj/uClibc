/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_memset
#define WANT_WIDE
#define Wmemset __wmemset

#include "wstring.c"

strong_alias(__wmemset, wmemset)

#undef L_memset
