/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <features.h>
#include <netdb.h>

#ifndef __UCLIBC_HAS_TLS__
# undef h_errno
extern int h_errno;
#endif

int *__h_errno_location(void)
{
    return &h_errno;
}
libc_hidden_def(__h_errno_location)
