/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <features.h>
#include <fcntl.h>
#include <stdarg.h>

#ifdef __UCLIBC_HAS_LFS__

#ifndef O_LARGEFILE
# define O_LARGEFILE	0100000
#endif

/* Open FILE with access OFLAG.  If OFLAG includes O_CREAT,
   a third argument is the file protection.  */
int open64 (const char *file, int oflag, ...)
{
    mode_t mode = 0;

    if (oflag & O_CREAT)
    {
	va_list arg;
	va_start (arg, oflag);
	mode = va_arg (arg, mode_t);
	va_end (arg);
    }

    return open(file, oflag | O_LARGEFILE, mode);
}
#ifndef __LINUXTHREADS_OLD__
libc_hidden_def(open64)
#else
libc_hidden_weak(open64)
strong_alias(open64,__libc_open64)
#endif

#endif /* __UCLIBC_HAS_LFS__ */
