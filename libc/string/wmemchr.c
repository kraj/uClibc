/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_memchr
#define WANT_WIDE
#define Wmemchr __wmemchr

#include "wstring.c"

strong_alias(__wmemchr, wmemchr)

#undef L_memchr
