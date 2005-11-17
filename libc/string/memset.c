/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_memset
#define Wmemset __memset

#include "wstring.c"

strong_alias(__memset, memset)

#undef L_memset
