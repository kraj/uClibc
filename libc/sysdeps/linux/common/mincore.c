/* 
 * Distributed under the terms of the GNU General Public License v2
 * $Header: /var/cvs/uClibc/libc/sysdeps/linux/common/mincore.c,v 1.1 2004/12/20 00:10:51 solar Exp $
 *
 * This file provides the mincore() system call to uClibc.
 * 20041215 - <solar@gentoo.org>
 *
 */

#include "syscalls.h"
#include <unistd.h>

#ifdef __NR_mincore
_syscall3(int, mincore, void *, start, size_t, length, unsigned char *, vec);
#endif
