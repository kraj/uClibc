/* brk on H8/300 by ysato */
/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

libc_hidden_proto(brk)

/* This must be initialized data because commons can't have aliases.  */
extern void *__curbrk;
libc_hidden_proto(__curbrk)
void *__curbrk = 0;
libc_hidden_data_def(__curbrk)

int brk (void *addr)
{
    void *newbrk;

    asm ("mov.l %2,er1\n\t"
	 "mov.l %1,er0\n\t"
	 "trapa #0\n\t"
	 "mov.l er0,%0"
	 : "=r" (newbrk)
	 : "0" (__NR_brk), "g" (addr)
	 : "er0","er1");

    __curbrk = newbrk;

    if (newbrk < addr)
    {
	__set_errno (ENOMEM);
	return -1;
    }

    return 0;
}
libc_hidden_def(brk)
