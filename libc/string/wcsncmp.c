/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strncmp
#define WANT_WIDE
#define Wstrncmp __wcsncmp

#include "wstring.c"

strong_alias(__wcsncmp, wcsncmp)

#undef L_strncmp
