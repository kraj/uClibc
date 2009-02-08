/*
 * Copyright (C) 2008-2009 Hai Zaar, Codefidence Ltd <haizaar@codefidence.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <dirent.h>
#include <string.h>
#include "dirstream.h"

int versionsort(const void *a, const void *b)
{
	return strverscmp((*(const struct dirent **) a)->d_name,
			(*(const struct dirent **) b)->d_name);
}
