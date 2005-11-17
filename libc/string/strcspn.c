/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strcspn
#define Wstrcspn __strcspn

#include "wstring.c"

strong_alias(__strcspn, strcspn)

#undef L_strcspn
