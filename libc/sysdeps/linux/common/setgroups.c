/* vi: set sw=4 ts=4: */
/*
 * setgroups() for uClibc
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

#define __NR___syscall_setgroups __NR_setgroups
static inline _syscall2(int, __syscall_setgroups,
		size_t, size, const __kernel_gid_t *, list);

int attribute_hidden __setgroups(size_t size, const gid_t *groups)
{
	if (size > (size_t) sysconf(_SC_NGROUPS_MAX)) {
ret_error:
		__set_errno(EINVAL);
		return -1;
	} else {
		size_t i;
		__kernel_gid_t *kernel_groups = NULL;

		if (size) {
			kernel_groups = (__kernel_gid_t *)malloc(sizeof(*kernel_groups) * size);
			if (kernel_groups == NULL)
				goto ret_error;
		}

		for (i = 0; i < size; i++) {
			kernel_groups[i] = (groups)[i];
			if (groups[i] != (gid_t) ((__kernel_gid_t) groups[i])) {
				goto ret_error;
			}
		}

		i = __syscall_setgroups(size, kernel_groups);
		if (kernel_groups)
			free(kernel_groups);
		return i;
	}
}
strong_alias(__setgroups,setgroups)
