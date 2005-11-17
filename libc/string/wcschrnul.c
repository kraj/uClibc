/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strchrnul
#define WANT_WIDE
#define Wstrchrnul __wcschrnul

#include "wstring.c"

strong_alias(__wcschrnul, wcschrnul)

#undef L_strchrnul
