/* vi: set sw=4 ts=4: */
/*
 * mknod syscall for uClibc
 *
 * Copyright (C) 2002 by Erik Andersen <andersen@codpoet.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#define _GNU_SOURCE
#define __FORCE_GLIBC
#include <features.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <asm/posix_types.h>

#ifndef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...) __syscall_mknod (args)
#define __NR___syscall_mknod __NR_mknod 
static inline _syscall3(int, __syscall_mknod, const char *, path, 
		unsigned short int, mode, unsigned short int, dev);
#endif

int __xmknod (int version, const char * path, __mode_t mode, __dev_t *dev)
{
	unsigned short int k_dev;
	/* We must convert the value to dev_t type used by the kernel.  */
	k_dev = ((major (*dev) & 0xff) << 8) | (minor (*dev) & 0xff);

	switch(version)
	{
		case 1:
			return INLINE_SYSCALL (mknod, 3, path, mode, k_dev);
		default:
			__set_errno(EINVAL);
			return -1;
	}
}

