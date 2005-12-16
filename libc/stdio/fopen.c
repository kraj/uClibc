/* Copyright (C) 2004       Manuel Novoa III    <mjn3@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 *
 * Dedicated to Toni.  See uClibc/DEDICATION.mjn3 for details.
 */

#include "_stdio.h"

#ifndef __DO_LARGEFILE
# define FILEDES_ARG    (-1)
#undef __fopen
#undef fopen
#else
#undef __fopen64
#undef fopen64
#endif

FILE attribute_hidden *__fopen(const char * __restrict filename, const char * __restrict mode)
{
	return _stdio_fopen(((intptr_t) filename), mode, NULL, FILEDES_ARG);
}
strong_alias(__fopen,fopen)
