/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_memcmp
#define Wmemcmp __memcmp

#include "wstring.c"

strong_alias(__memcmp, memcmp)

weak_alias(memcmp, bcmp)

#undef L_memcmp
