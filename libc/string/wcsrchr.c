/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strrchr
#define WANT_WIDE
#define Wstrrchr __wcsrchr

#include "wstring.c"

strong_alias(__wcsrchr, wcsrchr)

#undef L_strrchr
