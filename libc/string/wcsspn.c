/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strspn
#define WANT_WIDE
#define Wstrspn __wcsspn

#include "wstring.c"

strong_alias(__wcsspn, wcsspn)

#undef L_strspn
