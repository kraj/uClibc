/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strcmp
#define Wstrcmp __strcmp

#include "wstring.c"

strong_alias(__strcmp, strcmp)

#ifdef __LOCALE_C_ONLY
weak_alias(__strcmp, __strcoll)
strong_alias(__strcoll, strcoll)
#endif

#undef L_strcmp
