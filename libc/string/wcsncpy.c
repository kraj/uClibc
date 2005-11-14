/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strncpy
#define WANT_WIDE
#define Wstrncpy __wcsncpy

#include "wstring.c"

strong_alias(__wcsncpy, wcsncpy)

#undef L_strncpy
