/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <features.h>
#include <string.h>

char attribute_hidden *__glibc_strerror_r_internal(int errnum, char *strerrbuf, size_t buflen)
{
    __xpg_strerror_r_internal(errnum, strerrbuf, buflen);

    return strerrbuf;
}

strong_alias(__glibc_strerror_r_internal,__glibc_strerror_r)
/*hidden_weak_alias(__glibc_strerror_r_internal,__strerror_r)*/
