/*
 * Copyright (C) 2008-2009 Hai Zaar, Codefidence Ltd <haizaar@codefidence.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <_lfs_64.h>

#include <dirent.h>
#include <string.h>
#include "dirstream.h"

int versionsort64(const void *a, const void *b)
{
	return strverscmp((*(const struct dirent64 **) a)->d_name,
			(*(const struct dirent64 **) b)->d_name);
}
