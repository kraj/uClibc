/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strchr
#define WANT_WIDE
#define Wstrchr __wcschr

#include "wstring.c"

strong_alias(__wcschr, wcschr)

#undef L_strchr
