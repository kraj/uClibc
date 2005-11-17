/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strrchr
#define Wstrrchr __strrchr

#include "wstring.c"

strong_alias(__strrchr, strrchr)

weak_alias(strrchr, rindex)

#undef L_strrchr
