/* vi: set sw=4 ts=4: */
/*
 * Syscalls for uClibc
 *
 * Copyright (C) 2000 by Lineo, inc
 * Copyright (C) 2001 by Erik Andersen
 * Written by Erik Andersen <andersen@codpoet.org>
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

#include <errno.h>
#include <features.h>
#include <sys/types.h>
#include <sys/syscall.h>

//#define __NR_exit             1
#ifdef L__exit
/* Do not include unistd.h, so gcc doesn't whine about 
 * _exit returning.  It really doesn't return... */
#define __NR__exit __NR_exit
#ifdef __STR_NR_exit
#define __STR_NR__exit __STR_NR_exit
#endif
_syscall1(void, _exit, int, status);
#endif

//#define __NR_fork             2
#ifdef L_fork
#include <unistd.h>
#	ifdef __UCLIBC_HAS_MMU__
		_syscall0(pid_t, fork);
#	else
		pid_t fork(void)
		{
			__set_errno(ENOSYS);
			return -1;
		}
#	endif
#endif

//#define __NR_read             3
#ifdef L_read
#include <unistd.h>
_syscall3(ssize_t, read, int, fd, __ptr_t, buf, size_t, count);
#endif

//#define __NR_write            4
#ifdef L_write
#include <unistd.h>
_syscall3(ssize_t, write, int, fd, const __ptr_t, buf, size_t, count);
weak_alias(write, __write);
#endif

//#define __NR_open             5
#ifdef L___open
#include <stdarg.h>
#include <fcntl.h>
#define __NR___open __NR_open
#ifdef __STR_NR_open
#define __STR_NR___open __STR_NR_open
#endif
_syscall3(int, __open, const char *, fn, int, flags, mode_t, mode);

int open(const char *file, int oflag, ...)
{
	int mode = 0;

	if (oflag & O_CREAT) {
		va_list args;

		va_start(args, oflag);
		mode = va_arg(args, int);

		va_end(args);
	}

	return __open(file, oflag, mode);
}
#endif

//#define __NR_close            6
#ifdef L_close
#include <unistd.h>
_syscall1(int, close, int, fd);
#endif

//#define __NR_waitpid          7
// Implemented using wait4 

//#define __NR_creat            8
#ifdef L_creat
#include <fcntl.h>
_syscall2(int, creat, const char *, file, mode_t, mode);
#endif

//#define __NR_link             9
#ifdef L_link
#include <unistd.h>
_syscall2(int, link, const char *, oldpath, const char *, newpath);
#endif

//#define __NR_unlink           10
#ifdef L_unlink
#include <unistd.h>
_syscall1(int, unlink, const char *, pathname);
#endif

//#define __NR_execve           11
#ifdef L_execve
#include <unistd.h>
_syscall3(int, execve, const char *, filename, char *const *, argv,
		  char *const *, envp);
#endif

//#define __NR_chdir            12
#ifdef L_chdir
#include <unistd.h>
_syscall1(int, chdir, const char *, path);
#endif

//#define __NR_time             13
#ifdef L_time
#include <time.h>
_syscall1(time_t, time, time_t *, t);
#endif

//#define __NR_mknod            14
#ifdef L_mknod
#include <unistd.h>
extern int mknod(const char *pathname, mode_t mode, dev_t dev);
_syscall3(int, mknod, const char *, pathname, mode_t, mode, dev_t, dev);

int __xmknod (int version, const char * path, mode_t mode, dev_t *dev)
{
	switch(version)
	{
		case 1:
			return mknod (path, mode, *dev);
		default:
			__set_errno(EINVAL);
			return -1;
	}
}
#endif

//#define __NR_chmod            15
#ifdef L_chmod
#include <sys/stat.h>
_syscall2(int, chmod, const char *, path, mode_t, mode);
#endif

/* Old kernels don't have lchown -- do chown instead.  This
 * is sick and wrong, but at least things will compile.  
 * They may not follow links when they should though... */
#ifndef __NR_lchown 
#define __NR_lchown __NR_chown
#endif

//#define __NR_lchown           16
#ifdef L_lchown
#include <unistd.h>
_syscall3(int, lchown, const char *, path, uid_t, owner, gid_t, group);
#endif


//#define __NR_break            17

//#define __NR_oldstat          18

//#define __NR_lseek            19
#ifdef L_lseek
#include <unistd.h>
_syscall3(off_t, lseek, int, fildes, off_t, offset, int, whence);
#endif

//#define __NR_getpid           20
#ifdef L_getpid
#include <unistd.h>
_syscall0(pid_t, getpid);
#endif

//#define __NR_mount            21
#ifdef L_mount
#include <sys/mount.h>
_syscall5(int, mount, const char *, specialfile, const char *, dir,
		  const char *, filesystemtype, unsigned long, rwflag,
		  const void *, data);
#endif

//#define __NR_umount           22
#ifdef L_umount
#include <sys/mount.h>
_syscall1(int, umount, const char *, specialfile);
#endif

//#define __NR_setuid           23
#ifdef L_setuid
#include <unistd.h>
_syscall1(int, setuid, uid_t, uid);
#endif

//#define __NR_getuid           24
#ifdef L_getuid
#include <unistd.h>
_syscall0(gid_t, getuid);
#endif

//#define __NR_stime            25
#ifdef L_stime
#include <time.h>
_syscall1(int, stime, const time_t *, t);
#endif

//#define __NR_ptrace           26
#ifdef L___ptrace
#include <sys/ptrace.h>
#define __NR___ptrace __NR_ptrace
#ifdef __STR_NR_ptrace
#define __STR_NR___ptrace __STR_NR_ptrace
#endif
_syscall4(long, __ptrace, enum __ptrace_request, request, pid_t, pid,
		void*, addr, void*, data);
#endif


//#define __NR_alarm            27
#ifdef L_alarm
#include <unistd.h>
_syscall1(unsigned int, alarm, unsigned int, seconds);
#endif

//#define __NR_oldfstat         28

//#define __NR_pause            29
#ifdef L_pause
#include <unistd.h>
_syscall0(int, pause);
#endif

//#define __NR_utime            30
#ifdef L_utime
#include <utime.h>
_syscall2(int, utime, const char *, filename, const struct utimbuf *, buf);
#endif

//#define __NR_stty             31

//#define __NR_gtty             32

//#define __NR_access           33
#ifdef L_access
#include <unistd.h>
_syscall2(int, access, const char *, pathname, int, mode);
#endif

//#define __NR_nice             34
#ifdef L_nice
#include <unistd.h>
_syscall1(int, nice, int, inc);
#endif

//#define __NR_ftime            35

//#define __NR_sync             36
#ifdef L_sync
#include <unistd.h>
_syscall0(void, sync);
#endif

//#define __NR_kill             37
#ifdef L_kill
#include <signal.h>
#undef kill
_syscall2(int, kill, pid_t, pid, int, sig);
#endif

//#define __NR_rename           38
#ifdef L_rename
#include <stdio.h>
_syscall2(int, rename, const char *, oldpath, const char *, newpath);
#endif

//#define __NR_mkdir            39
#ifdef L_mkdir
#include <sys/stat.h>
_syscall2(int, mkdir, const char *, pathname, mode_t, mode);
#endif

//#define __NR_rmdir            40
#ifdef L_rmdir
#include <unistd.h>
_syscall1(int, rmdir, const char *, pathname);
#endif

//#define __NR_dup              41
#ifdef L_dup
#include <unistd.h>
_syscall1(int, dup, int, oldfd);
#endif

//#define __NR_pipe             42
#ifdef L_pipe
#include <unistd.h>
/*
 * SH has a weird register calling mechanism for pipe, see pipe.c
 */
#if !defined(__sh__)
_syscall1(int, pipe, int *, filedes);
#endif
#endif

//#define __NR_times            43
#ifdef L_times
#include <sys/times.h>
_syscall1(clock_t, times, struct tms *, buf);
#endif

//#define __NR_prof             44

//#define __NR_brk              45

//#define __NR_setgid           46
#ifdef L_setgid
#include <unistd.h>
_syscall1(int, setgid, gid_t, gid);
#endif

//#define __NR_getgid           47
#ifdef L_getgid
#include <unistd.h>
_syscall0(gid_t, getgid);
#endif

//#define __NR_signal           48

//#define __NR_geteuid          49
#ifdef	L_geteuid
#	ifdef	__NR_geteuid
#	include <unistd.h>
	_syscall0(uid_t, geteuid);
#	else
	uid_t geteuid(void)
	{
		return (getuid());
	}
#	endif
#endif

//#define __NR_getegid          50
#ifdef	L_getegid
#	ifdef	__NR_getegid
#	include <unistd.h>
	_syscall0(gid_t, getegid);
#	else
	gid_t getegid(void)
	{
		return (getgid());
	}
#	endif
#endif

//#define __NR_acct             51

//#define __NR_umount2          52
#ifdef L_umount2
#	ifdef __NR_umount2 /* Old kernels don't have umount2 */ 
#		include <sys/mount.h>
		_syscall2(int, umount2, const char *, special_file, int, flags);
#	else
		int umount2(const char * special_file, int flags)
		{
			__set_errno(ENOSYS);
			return -1;
		}
#	endif
#endif

//#define __NR_lock             53

//#define __NR_ioctl            54
#ifdef L__ioctl
#include <stdarg.h>
#include <sys/ioctl.h>
#define __NR__ioctl __NR_ioctl
#ifdef __STR_NR_ioctl
#define __STR_NR__ioctl __STR_NR_ioctl
#endif
extern int _ioctl(int fd, int request, void *arg);

_syscall3(int, _ioctl, int, fd, int, request, void *, arg);

int ioctl(int fd, unsigned long int request, ...)
{
	void *arg;
	va_list list;

	va_start(list, request);
	arg = va_arg(list, void *);

	va_end(list);
	return _ioctl(fd, request, arg);
}
#endif

//#define __NR_fcntl            55
#ifdef L__fcntl
#include <stdarg.h>
#include <fcntl.h>
#define __NR__fcntl __NR_fcntl
#ifdef __STR_NR_fcntl
#define __STR_NR__fcntl __STR_NR_fcntl
#endif
extern int _fcntl(int fd, int cmd, long arg);

_syscall3(int, _fcntl, int, fd, int, cmd, long, arg);

int fcntl(int fd, int command, ...)
{
	long arg;
	va_list list;

	va_start(list, command);
	arg = va_arg(list, long);

	va_end(list);
	return _fcntl(fd, command, arg);
}
#endif

//#define __NR_mpx              56

//#define __NR_setpgid          57
#ifdef L_setpgid
#include <unistd.h>
_syscall2(int, setpgid, pid_t, pid, pid_t, pgid);
#endif

//#define __NR_ulimit           58

//#define __NR_oldolduname      59

//#define __NR_umask            60
#ifdef L_umask
#include <sys/stat.h>
_syscall1(mode_t, umask, mode_t, mask);
#endif

//#define __NR_chroot           61
#ifdef L_chroot
#include <unistd.h>
_syscall1(int, chroot, const char *, path);
#endif

//#define __NR_ustat            62

//#define __NR_dup2             63
#ifdef L_dup2
#include <unistd.h>
_syscall2(int, dup2, int, oldfd, int, newfd);
#endif

//#define __NR_getppid          64
#ifdef	L_getppid
#	include <unistd.h>
#	ifdef	__NR_getppid
	_syscall0(pid_t, getppid);
#	else
	pid_t getppid(void)
	{
		return (getpid());
	}
#	endif
#endif

//#define __NR_getpgrp          65
#ifdef L_getpgrp
#include <unistd.h>
_syscall0(pid_t, getpgrp);
#endif

//#define __NR_setsid           66
#ifdef L_setsid
#include <unistd.h>
_syscall0(pid_t, setsid);
#endif

//#define __NR_sigaction        67
#ifdef L_sigaction
#include <signal.h>
#undef sigaction
_syscall3(int, sigaction, int, signum, const struct sigaction *, act,
		  struct sigaction *, oldact);
#endif

//#define __NR_sgetmask         68

//#define __NR_ssetmask         69

//#define __NR_setreuid         70
#ifdef L_setreuid
#include <unistd.h>
_syscall2(int, setreuid, uid_t, ruid, uid_t, euid);
#endif

//#define __NR_setregid         71
#ifdef L_setregid
#include <unistd.h>
_syscall2(int, setregid, gid_t, rgid, gid_t, egid);
#endif

//#define __NR_sigsuspend       72
#ifdef L_sigsuspend
#include <signal.h>
#undef sigsuspend
_syscall1(int, sigsuspend, const sigset_t *, mask);
#endif

//#define __NR_sigpending       73
#ifdef L_sigpending
#include <signal.h>
#undef sigpending
_syscall1(int, sigpending, sigset_t *, set);
#endif

//#define __NR_sethostname      74
#ifdef L_sethostname
#include <unistd.h>
_syscall2(int, sethostname, const char *, name, size_t, len);
#endif

//#define __NR_setrlimit        75
#ifdef L_setrlimit
#include <unistd.h>
#include <sys/resource.h>
_syscall2(int, setrlimit, int, resource, const struct rlimit *, rlim);
#endif

//#define __NR_getrlimit        76
#ifdef L_getrlimit
#include <unistd.h>
#include <sys/resource.h>
_syscall2(int, getrlimit, int, resource, struct rlimit *, rlim);
#endif

//#define __NR_getrusage        77
#ifdef L_getrusage
#include <unistd.h>
#include <wait.h>
_syscall2(int, getrusage, int, who, struct rusage *, usage);
#endif

//#define __NR_gettimeofday     78
#ifdef L_gettimeofday
#include <sys/time.h>
_syscall2(int, gettimeofday, struct timeval *, tv, struct timezone *, tz);
#endif

//#define __NR_settimeofday     79
#ifdef L_settimeofday
#include <sys/time.h>
_syscall2(int, settimeofday, const struct timeval *, tv,
		  const struct timezone *, tz);
#endif

//#define __NR_getgroups        80
#ifdef L_getgroups
#include <unistd.h>
_syscall2(int, getgroups, int, size, gid_t *, list);
#endif

//#define __NR_setgroups        81
#ifdef L_setgroups
#include <unistd.h>
#include <grp.h>
_syscall2(int, setgroups, size_t, size, const gid_t *, list);
#endif

//#define __NR_select           82

//#define __NR_symlink          83
#ifdef L_symlink
#include <unistd.h>
_syscall2(int, symlink, const char *, oldpath, const char *, newpath);
#endif

//#define __NR_oldlstat         84

//#define __NR_readlink         85
#ifdef L_readlink
#include <unistd.h>
_syscall3(int, readlink, const char *, path, char *, buf, size_t, bufsiz);
#endif

//#define __NR_uselib           86
#ifdef L_uselib
#include <unistd.h>
_syscall1(int, uselib, const char *, library);
#endif

//#define __NR_swapon           87
#ifdef L_swapon
#include <sys/swap.h>
_syscall2(int, swapon, const char *, path, int, swapflags);
#endif

//#define __NR_reboot           88
#ifdef L__reboot
#define __NR__reboot __NR_reboot
#ifdef __STR_NR_reboot
#define __STR_NR__reboot __STR_NR_reboot
#endif
extern int _reboot(int magic, int magic2, int flag);

_syscall3(int, _reboot, int, magic, int, magic2, int, flag);

int reboot(int flag)
{
	return (_reboot((int) 0xfee1dead, 672274793, flag));
}
#endif

//#define __NR_readdir          89

//#define __NR_mmap             90
#ifdef L_mmap
#define __NR__mmap __NR_mmap
#ifdef __STR_NR_mmap
#define __STR_NR__mmap __STR_NR_mmap
#endif
#include <unistd.h>
#include <sys/mman.h>

#if __UCLIBC_OLD_STYLE_MMAP__

extern __ptr_t _mmap(unsigned long *buffer);

_syscall1(__ptr_t, _mmap, unsigned long *, buffer);

__ptr_t mmap(__ptr_t addr, size_t len, int prot,
			 int flags, int fd, __off_t offset)
{
	unsigned long buffer[6];

	buffer[0] = (unsigned long) addr;
	buffer[1] = (unsigned long) len;
	buffer[2] = (unsigned long) prot;
	buffer[3] = (unsigned long) flags;
	buffer[4] = (unsigned long) fd;
	buffer[5] = (unsigned long) offset;
	return (__ptr_t) _mmap(buffer);
}

#else /* !__UCLIBC_OLD_STYLE_MMAP__ */

_syscall6(__ptr_t, mmap,
	  __ptr_t, addr, size_t, len, int, prot,
	  int, flags, int, fd, __off_t, offset);

#endif /* __UCLIBC_OLD_STYLE_MMAP__ */

#endif

//#define __NR_munmap           91
#ifdef L_munmap
#include <unistd.h>
#include <sys/mman.h>
_syscall2(int, munmap, void *, start, size_t, length);
#endif

//#define __NR_truncate         92
#ifdef L_truncate
#include <unistd.h>
_syscall2(int, truncate, const char *, path, off_t, length);
#endif

//#define __NR_ftruncate        93
#ifdef L_ftruncate
#include <unistd.h>
_syscall2(int, ftruncate, int, fd, off_t, length);
#endif

//#define __NR_fchmod           94
#ifdef L_fchmod
#include <sys/stat.h>
_syscall2(int, fchmod, int, fildes, mode_t, mode);
#endif

//#define __NR_fchown           95
#ifdef L_fchown
#include <unistd.h>
_syscall3(int, fchown, int, fd, uid_t, owner, gid_t, group);
#endif

//#define __NR_getpriority      96
#ifdef L_getpriority
#include <sys/resource.h>
_syscall2(int, getpriority, __priority_which_t, which, id_t, who);
#endif

//#define __NR_setpriority      97
#ifdef L_setpriority
#include <sys/resource.h>
_syscall3(int, setpriority, __priority_which_t, which, id_t, who, int, prio);
#endif

//#define __NR_profil           98

//#define __NR_statfs           99
#ifdef L_statfs
#include <sys/vfs.h>
_syscall2(int, statfs, const char *, path, struct statfs *, buf);
#endif

//#define __NR_fstatfs          100
#ifdef L_fstatfs
#include <sys/vfs.h>
_syscall2(int, fstatfs, int, fd, struct statfs *, buf);
#endif

//#define __NR_ioperm           101
#ifdef L_ioperm
#include <sys/io.h>
#	if defined __UCLIBC_HAS_MMU__ && defined __NR_ioperm
		_syscall3(int, ioperm, unsigned long, from, unsigned long, num, int, turn_on);
#	else
		int ioperm(unsigned long from, unsigned long num, int turn_on)
		{
			__set_errno(ENOSYS);
			return -1;
		}
#	endif
#endif

//#define __NR_socketcall       102
#ifdef L_socketcall
_syscall2(int, socketcall, int, call, unsigned long *, args);
#endif

//#define __NR_syslog           103
#ifdef L__syslog
#include <unistd.h>
#define __NR__syslog		__NR_syslog
#ifdef __STR_NR_syslog
#define __STR_NR__syslog	__STR_NR_syslog
#endif
extern int _syslog(int type, char *buf, int len);

_syscall3(int, _syslog, int, type, char *, buf, int, len);

int klogctl(int type, char *buf, int len)
{
	return (_syslog(type, buf, len));
}

#endif

//#define __NR_setitimer        104
#ifdef L_setitimer
#include <sys/time.h>
_syscall3(int, setitimer, __itimer_which_t, which,
		  const struct itimerval *, new, struct itimerval *, old);
#endif

//#define __NR_getitimer        105
#ifdef L_getitimer
#include <sys/time.h>
_syscall2(int, getitimer, __itimer_which_t, which, struct itimerval *, value);
#endif

//#define __NR_stat             106
#ifdef L___stat
#include <unistd.h>
#include "statfix.h"
#define __NR___stat	__NR_stat
#ifdef __STR_NR_stat
#define __STR_NR___stat	__STR_NR_stat
#endif
extern int __stat(const char *file_name, struct kernel_stat *buf);
_syscall2(int, __stat, const char *, file_name, struct kernel_stat *, buf);

int __xstat(int version, const char * file_name, struct libc_stat * cstat)
{
	struct kernel_stat kstat;
	int result = __stat(file_name, &kstat);

	if (result == 0) { 
		statfix(cstat, &kstat);
	}
	return result;
}

int stat(const char *file_name, struct libc_stat *buf)
{
	return(__xstat(0, file_name, buf));
}
#endif

//#define __NR_lstat            107
#ifdef L___lstat
#include <unistd.h>
#include "statfix.h"
#define __NR___lstat	__NR_lstat
#ifdef __STR_NR_lstat
#define __STR_NR___lstat	__STR_NR_lstat
#endif
extern int __lstat(const char *file_name, struct kernel_stat *buf);
_syscall2(int, __lstat, const char *, file_name, struct kernel_stat *, buf);

int __lxstat(int version, const char * file_name, struct libc_stat * cstat)
{
	struct kernel_stat kstat;
	int result = __lstat(file_name, &kstat);

	if (result == 0) { 
		statfix(cstat, &kstat);
	}
	return result;
}

int lstat(const char *file_name, struct libc_stat *buf)
{
	return(__lxstat(0, file_name, buf));
}
#endif

//#define __NR_fstat            108
#ifdef L___fstat
#include <unistd.h>
#include "statfix.h"
#define __NR___fstat	__NR_fstat
#ifdef __STR_NR_fstat
#define __STR_NR___fstat	__STR_NR_fstat
#endif
extern int __fstat(int filedes, struct kernel_stat *buf);
_syscall2(int, __fstat, int, filedes, struct kernel_stat *, buf);

int __fxstat(int version, int fd, struct libc_stat * cstat)
{
	struct kernel_stat kstat;
	int result = __fstat(fd, &kstat);

	if (result == 0) { 
		statfix(cstat, &kstat);
	}
	return result;
}

int fstat(int filedes, struct libc_stat *buf)
{
	return(__fxstat(0, filedes, buf));
}
#endif

//#define __NR_olduname         109

//#define __NR_iopl             110
#ifdef L_iopl
#include <sys/io.h>
#	if defined __UCLIBC_HAS_MMU__ && defined __NR_iopl
		_syscall1(int, iopl, int, level);
#	else
		int iopl(int level)
		{
			__set_errno(ENOSYS);
			return -1;
		}
#	endif
#endif

//#define __NR_vhangup          111
#ifdef L_vhangup
#include <unistd.h>
_syscall0(int, vhangup);
#endif

//#define __NR_idle             112
//int idle(void);

//#define __NR_vm86old          113

//#define __NR_wait4            114
#ifdef L_wait4
_syscall4(int, wait4, pid_t, pid, int *, status, int, opts, void *, rusage);
#endif

//#define __NR_swapoff          115
#ifdef L_swapoff
#include <sys/swap.h>
_syscall1(int, swapoff, const char *, path);
#endif

//#define __NR_sysinfo          116
#ifdef L_sysinfo
#include <sys/sysinfo.h>
_syscall1(int, sysinfo, struct sysinfo *, info);
#endif

//#define __NR_ipc              117
#ifdef L___ipc
#define __NR___ipc __NR_ipc
#ifdef __STR_NR_ipc
#define __STR_NR___ipc __STR_NR_ipc
#endif
_syscall5(int, __ipc, unsigned int, call, int, first, int, second, int, third, void *, ptr);
#endif

//#define __NR_fsync            118
#ifdef L_fsync
#include <unistd.h>
_syscall1(int, fsync, int, fd);
#endif

//#define __NR_sigreturn        119
//int sigreturn(unsigned long __unused);

//#define __NR_clone            120
//See architecture specific implementation...

//#define __NR_setdomainname    121
#ifdef L_setdomainname
#include <unistd.h>
_syscall2(int, setdomainname, const char *, name, size_t, len);
#endif

//#define __NR_uname            122
#ifdef L_uname
#include <sys/utsname.h>
_syscall1(int, uname, struct utsname *, buf);
#endif

//#define __NR_modify_ldt       123

//#define __NR_adjtimex         124
#ifdef L_adjtimex
#include <sys/timex.h>
_syscall1(int, adjtimex, struct timex *, buf);
#endif

//#define __NR_mprotect         125
#ifdef L_mprotect
#include <sys/mman.h>
_syscall3(int, mprotect, void *, addr, size_t, len, int, prot);
#endif

//#define __NR_sigprocmask      126
#ifdef L_sigprocmask
#include <signal.h>
#undef sigprocmask
_syscall3(int, sigprocmask, int, how, const sigset_t *, set, sigset_t *,
		  oldset);
#endif

//#define __NR_create_module    127
//See sysdeps/linux/commom/create_module.c

//#define __NR_init_module      128
#ifdef L_init_module
/* This may have 5 arguments (for old 2.0 kernels) or 2 arguments
 * (for 2.2 and 2.4 kernels).  Use the greatest common denominator,
 * and let the kernel cope with whatever it gets.  It's good at that. */
_syscall5(int, init_module, void *, first, void *, second, void *, third, 
			void *, fourth, void *, fifth);
#endif

//#define __NR_delete_module    129
#ifdef L_delete_module
#	ifdef __NR_delete_module
		_syscall1(int, delete_module, const char *, name);
#	else
		int delete_module(const char * name)
		{
			__set_errno(ENOSYS);
			return -1;
		}
#	endif
#endif

//#define __NR_get_kernel_syms  130

//#define __NR_quotactl         131

//#define __NR_getpgid          132
#ifdef L_getpgid
_syscall1(pid_t, getpgid, pid_t, pid);
#endif

//#define __NR_fchdir           133
#ifdef L_fchdir
#include <unistd.h>
_syscall1(int, fchdir, int, fd);
#endif

//#define __NR_bdflush          134
#ifdef L_bdflush
#include <sys/kdaemon.h>

_syscall2(int, bdflush, int, __func, long int, __data);
#endif

//#define __NR_sysfs            135

//#define __NR_personality      136

//#define __NR_afs_syscall      137

//#define __NR_setfsuid         138

//#define __NR_setfsgid         139

//#define __NR__llseek          140
#ifdef L__llseek
extern int _llseek(int fd, off_t hoff, off_t loff, loff_t *res, int whence);

_syscall5(int, _llseek, int, fd, off_t, hoff, off_t, loff, loff_t *, res,
		  int, whence);

loff_t llseek(int fd, loff_t offset, int whence)
{
	int ret;
	loff_t result;

	ret = _llseek(fd, (off_t) (offset >> 32),
				  (off_t) (offset & 0xffffffff), &result, whence);

	return ret ? (loff_t) ret : result;
}
#endif

//#define __NR_getdents         141
#ifdef L_getdents
#include <unistd.h>
#include <dirent.h>
_syscall3(int, getdents, int, fd, char *, dirp, size_t, count);
#endif

//#define __NR__newselect       142
#ifdef L__newselect
#include <unistd.h>
extern int _newselect(int n, fd_set *readfds, fd_set *writefds,
					  fd_set *exceptfds, struct timeval *timeout);

_syscall5(int, _newselect, int, n, fd_set *, readfds, fd_set *, writefds,
		  fd_set *, exceptfds, struct timeval *, timeout);

int select(int n, fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
		   struct timeval *timeout)
{
	return (_newselect(n, readfds, writefds, exceptfds, timeout));
}
#endif

//#define __NR_flock            143
#ifdef L_flock
#include <sys/file.h>
_syscall2(int,flock,int,fd, int,operation);
#endif

//#define __NR_msync            144
//#define __NR_readv            145

#ifdef L_readv
#include <sys/uio.h>
_syscall3(ssize_t, readv, int, filedes, const struct iovec *, vector, int,
		  count);
#endif

//#define __NR_writev           146
#ifdef L_writev
#include <sys/uio.h>
_syscall3(ssize_t, writev, int, filedes, const struct iovec *, vector, int,
		  count);
#endif

//#define __NR_getsid           147
#ifdef L_getsid
#include <unistd.h>
_syscall1(pid_t, getsid, pid_t, pid);
#endif

//#define __NR_fdatasync        148
//#define __NR__sysctl          149

//#define __NR_mlock            150
#ifdef L_mlock
#include <sys/mman.h>
#	if defined __UCLIBC_HAS_MMU__ && defined __NR_mlock
		_syscall2(int, mlock, const void *, addr, size_t, len);
#	endif	
#endif	

//#define __NR_munlock          151
#ifdef L_munlock
#include <sys/mman.h>
#	if defined __UCLIBC_HAS_MMU__ && defined __NR_munlock
		_syscall2(int, munlock, const void *, addr, size_t, len);
#	endif	
#endif	

//#define __NR_mlockall         152
#ifdef L_mlockall
#include <sys/mman.h>
#	if defined __UCLIBC_HAS_MMU__ && defined __NR_mlockall
		_syscall1(int, mlockall, int, flags);
#	endif	
#endif	

//#define __NR_munlockall       153
#ifdef L_munlockall
#include <sys/mman.h>
#	if defined __UCLIBC_HAS_MMU__ && defined L_munlockall
		_syscall0(int, munlockall);
#	endif	
#endif	

//#define __NR_sched_setparam   154
//#define __NR_sched_getparam   155
//#define __NR_sched_setscheduler       156
//#define __NR_sched_getscheduler       157
//#define __NR_sched_yield              158
//#define __NR_sched_get_priority_max   159
//#define __NR_sched_get_priority_min   160
//#define __NR_sched_rr_get_interval    161

//#define __NR_nanosleep                162
#ifdef L_nanosleep
#include <time.h>
_syscall2(int, nanosleep, const struct timespec *, req, struct timespec *, rem);
#endif

//#define __NR_mremap                   163
#ifdef L_mremap
#include <unistd.h>
#include <sys/mman.h>
_syscall4(__ptr_t, mremap, __ptr_t, old_address, size_t, old_size, size_t,
		  new_size, int, may_move);
#endif

//#define __NR_setresuid                164
//#define __NR_getresuid                165
//#define __NR_vm86                     166

//#define __NR_query_module             167
#ifdef L_query_module
#	ifdef __NR_query_module
		_syscall5(int, query_module, const char *, name, int, which,
				void *, buf, size_t, bufsize, size_t*, ret);
#	else
		int query_module(const char * name, int which,
					void * buf, size_t bufsize, size_t* ret)
		{
			__set_errno(ENOSYS);
			return -1;
		}
#	endif	
#endif	

//#define __NR_poll                     168
#if defined(L_poll) && defined(__NR_poll) /* uClinux 2.0 doesn't have poll */
#include <sys/poll.h>
_syscall3(int, poll, struct pollfd *, fds, unsigned long int, nfds, int, timeout);
#endif

//#define __NR_nfsservctl               169
//#define __NR_setresgid                170
//#define __NR_getresgid                171
//#define __NR_prctl                    172
//#define __NR_rt_sigreturn             173
//#define __NR_rt_sigaction             174
//#define __NR_rt_sigprocmask           175
//#define __NR_rt_sigpending            176
//#define __NR_rt_sigtimedwait          177
//#define __NR_rt_sigqueueinfo          178
//#define __NR_rt_sigsuspend            179
//#define __NR_pread                    180
//#define __NR_pwrite                   181
//#define __NR_chown                    182
#ifdef L_chown
#include <unistd.h>
_syscall3(int, chown, const char *, path, uid_t, owner, gid_t, group);
#endif

//#define __NR_getcwd                   183
// See unistd/getcwd.c -- we don't use this syscall, even when it is available...

//#define __NR_capget                   184
#ifdef L_capget
#	ifdef __NR_capget
		_syscall2(int, capget, void*, header, void*, data);
#	else
		int capget(void* header, void* data)
		{
			__set_errno(ENOSYS);
			return -1;
		}
#	endif
#endif

//#define __NR_capset                   185
#ifdef L_capset
#	ifdef __NR_capset
		_syscall2(int, capset, void*, header, const void*, data);
#	else
		int capset(void* header, const void* data)
		{
			__set_errno(ENOSYS);
			return -1;
		}
#	endif
#endif

//#define __NR_sigaltstack              186

//#define __NR_sendfile                 187

//#define __NR_getpmsg                  188

//#define __NR_putpmsg                  189

//#define __NR_vfork                    190
//See sysdeps/linux/<arch>vfork.[cS] for architecture specific implementation...


