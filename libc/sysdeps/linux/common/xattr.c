/* 
 * Distributed under the terms of the GNU General Public License v2
 * $Header: /var/cvs/uClibc/libc/sysdeps/linux/common/xattr.c,v 1.1 2004/12/20 00:10:51 solar Exp $
 *
 * This file provides the following Extended Attribute system calls to uClibc.
 *
 *	setxattr(), lsetxattr(), fsetxattr(),
 *	getxattr(), lgetxattr(), fgetxattr(),
 *	listxattr(), llistxattr(), flistxattr(),
 *	removexattr(), lremovexattr(), fremovexattr()
 *
 * 20041215 - <solar@gentoo.org>
 */

#include "syscalls.h"
#include <unistd.h>

#ifdef __NR_setxattr
_syscall5(int, setxattr, const char *, path, const char *, name, const void *,
		  value, size_t, size, int, flags);
#endif

#ifdef __NR_lsetxattr
_syscall5(int, lsetxattr, const char *, path, const char *, name, const void *,
		  value, size_t, size, int, flags);
#endif

#ifdef __NR_fsetxattr
_syscall5(int, fsetxattr, int, filedes, const char *, name, const void *,
		  value, size_t, size, int, flags);
#endif

#ifdef __NR_getxattr
_syscall4(ssize_t, getxattr, const char *, path, const char *, name,
		  void *, value, size_t, size);
#endif

#ifdef __NR_lgetxattr
_syscall4(ssize_t, lgetxattr, const char *, path, const char *, name, void *,
		  value, size_t, size);
#endif

#ifdef __NR_fgetxattr
_syscall4(ssize_t, fgetxattr, int, filedes, const char *, name, void *, value,
		  size_t, size);
#endif

#ifdef __NR_listxattr
_syscall3(ssize_t, listxattr, const char *, path, char *, list, size_t, size);
#endif

#ifdef __NR_llistxattr
_syscall3(ssize_t, llistxattr, const char *, path, char *, list, size_t,
		  size);
#endif

#ifdef __NR_flistxattr
_syscall3(size_t, flistxattr, int, filedes, char *, list, size_t, size);
#endif

#ifdef __NR_removexattr
_syscall2(int, removexattr, const char *, path, const char *, name);
#endif

#ifdef __NR_lremovexattr
_syscall2(int, lremovexattr, const char *, path, const char *, name);
#endif

#ifdef __NR_fremovexattr
_syscall2(int, fremovexattr, int, filedes, const char *, name);
#endif
