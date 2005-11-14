/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_strcat
#define Wstrcat __strcat

#include "wstring.c"

strong_alias(__strcat, strcat)

#undef L_strcat
