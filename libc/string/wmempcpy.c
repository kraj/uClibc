/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_mempcpy
#define WANT_WIDE
#define Wmempcpy __wmempcpy

#include "wstring.c"

strong_alias(__wmempcpy, wmempcpy)

#undef L_mempcpy
