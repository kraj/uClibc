/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strcpy
#define WANT_WIDE
#define Wstrcpy __wcscpy

#include "wstring.c"

strong_alias(__wcscpy, wcscpy)

#undef L_strcpy
