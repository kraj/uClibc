/*
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define L_stpncpy
#define WANT_WIDE
#define Wstpncpy __wcpncpy

#include "wstring.c"

strong_alias(__wcpncpy, wcpncpy)

#undef L_stpncpy
