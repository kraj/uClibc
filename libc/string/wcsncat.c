/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strncat
#define WANT_WIDE
#define Wstrncat __wcsncat

#include "wstring.c"

strong_alias(__wcsncat, wcsncat)

#undef L_strncat
