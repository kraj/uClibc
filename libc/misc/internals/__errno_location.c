/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <features.h>
#include <errno.h>

#ifndef __UCLIBC_HAS_TLS__
# undef errno
extern int errno;
#endif

int *__errno_location(void)
{
    return &errno;
}
libc_hidden_def(__errno_location)
