/* Copyright (C) 2004       Manuel Novoa III    <mjn3@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 *
 * Dedicated to Toni.  See uClibc/DEDICATION.mjn3 for details.
 */

#include "_stdio.h"

#ifdef __UCLIBC_HAS_LFS__
# define __DO_LARGEFILE
# define freopen		freopen64
# define FILEDES_ARG    (-2)
# include "freopen.c"
#endif
