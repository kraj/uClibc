/* Copyright (C) 2004       Manuel Novoa III    <mjn3@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 *
 * Dedicated to Toni.  See uClibc/DEDICATION.mjn3 for details.
 */

#include "_stdio.h"

#ifdef __UCLIBC_HAS_LFS__
# define __DO_LARGEFILE
# define fgetpos	fgetpos64
# define fpos_t		fpos64_t
# define ftell		ftello64
# include "fgetpos.c"
#endif
