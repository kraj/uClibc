/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strstr
#define WANT_WIDE
#define Wstrstr __wcsstr

#include "wstring.c"

strong_alias(__wcsstr, wcsstr)

weak_alias(wcsstr, wcswcs)

#undef L_strstr
