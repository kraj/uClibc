/* vi: set sw=4 ts=4: */
/*
 * setuid syscall for uClibc
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
#include <features.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>

#ifndef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...) __syscall_setuid (args)
#define __NR___syscall_setuid __NR_setuid 
static inline _syscall1(int, __syscall_setuid, int, id);
#endif

int setuid(uid_t uid)
{
	if (uid == (uid_t) ~0 || uid != (uid_t) ((int) uid)) {
		__set_errno (EINVAL);
		return -1;
	}

	return(INLINE_SYSCALL(setuid, 1, uid));
}

