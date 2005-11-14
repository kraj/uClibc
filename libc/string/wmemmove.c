/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_memmove
#define WANT_WIDE
#define Wmemmove __wmemmove

#include "wstring.c"

strong_alias(__wmemmove, wmemmove)

#undef L_memmove
