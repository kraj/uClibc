/* Copyright (C) 2004       Manuel Novoa III    <mjn3@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 *
 * Dedicated to Toni.  See uClibc/DEDICATION.mjn3 for details.
 */

#include "_stdio.h"

#ifdef __UCLIBC_HAS_LFS__
# define __DO_LARGEFILE
# define FTELL			__ftello64
# define OFFSET_TYPE		__off64_t
# include "ftello.c"
#endif
