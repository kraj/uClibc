/* vi: set sw=4 ts=4: */
/*
 * getgroups() for uClibc
 *
 * Copyright (C) 2000-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 */

#define sysconf __sysconf

#include "syscalls.h"
#include <stdlib.h>
#include <unistd.h>
#include <grp.h>

#define MIN(a,b) (((a)<(b))?(a):(b))

#define __NR___syscall_getgroups __NR_getgroups
static inline _syscall2(int, __syscall_getgroups,
		int, size, __kernel_gid_t *, list);

int attribute_hidden __getgroups(int size, gid_t groups[])
{
	if (unlikely(size < 0)) {
ret_error:
		__set_errno(EINVAL);
		return -1;
	} else {
		int i, ngids;
		__kernel_gid_t *kernel_groups;

		size = MIN(size, sysconf(_SC_NGROUPS_MAX));
		kernel_groups = (__kernel_gid_t *)malloc(sizeof(*kernel_groups) * size);
		if (size && kernel_groups == NULL)
			goto ret_error;

		ngids = __syscall_getgroups(size, kernel_groups);
		if (size != 0 && ngids > 0) {
			for (i = 0; i < ngids; i++) {
				groups[i] = kernel_groups[i];
			}
		}

		if (kernel_groups)
			free(kernel_groups);
		return ngids;
	}
}
strong_alias(__getgroups,getgroups)
