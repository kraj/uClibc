/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#define __FORCE_GLIBC
#include <features.h>
#include <netdb.h>
#undef h_errno
extern int h_errno;

libc_hidden_proto(h_errno)

libc_hidden_proto(__h_errno_location)
int * weak_const_function __h_errno_location (void)
{
    return &h_errno;
}
libc_hidden_weak(__h_errno_location)
