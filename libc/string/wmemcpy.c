/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_memcpy
#define WANT_WIDE
#define Wmemcpy __wmemcpy

#include "wstring.c"

strong_alias(__wmemcpy, wmemcpy)

#undef L_memcpy
