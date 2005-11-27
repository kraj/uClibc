/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strtok_r
#define Wstrtok_r __strtok_r

#undef Wstrspn
#define Wstrspn __strspn
#undef Wstrpbrk
#define Wstrpbrk __strpbrk

#include "wstring.c"

strong_alias(__strtok_r, strtok_r)

#undef L_strtok_r
