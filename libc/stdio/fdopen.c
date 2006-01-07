/* Copyright (C) 2004       Manuel Novoa III    <mjn3@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 *
 * Dedicated to Toni.  See uClibc/DEDICATION.mjn3 for details.
 */

#include "_stdio.h"

FILE attribute_hidden *__fdopen(int filedes, const char *mode)
{
	intptr_t cur_mode;

	return (((cur_mode = __fcntl(filedes, F_GETFL))) != -1)
		? _stdio_fopen(cur_mode, mode, NULL, filedes) 
		: NULL;
}
strong_alias(__fdopen,fdopen)
