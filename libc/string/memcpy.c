/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_memcpy
#define Wmemcpy __memcpy

#include "wstring.c"

strong_alias(__memcpy, memcpy)

#undef L_memcpy
