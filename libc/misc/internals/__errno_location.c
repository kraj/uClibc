/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "internal_errno.h"

int *__errno_location(void)
{
    return &errno;
}
libc_hidden_weak(__errno_location)
