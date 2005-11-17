/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strdup
#define Wstrdup __strdup

#undef Wstrlen
#undef Wstrcpy
#define Wstrlen strlen
#define Wstrcpy strcpy

#include "wstring.c"

strong_alias(__strdup, strdup)

#undef L_strdup
