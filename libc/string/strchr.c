/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strchr
#define Wstrchr __strchr

#include "wstring.c"

strong_alias(__strchr, strchr)

weak_alias(strchr, index)

#undef L_strchr
