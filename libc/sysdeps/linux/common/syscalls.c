/* vi: set sw=4 ts=4: */
/*
 * Syscalls for uClibc
 *
 * Copyright (C) 2000 by Lineo, inc
 * Copyright (C) 2001, 2002 by Erik Andersen
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

#include <features.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>

//#define __NR_exit             1
#ifdef L__exit
/* Do not include unistd.h, so gcc doesn't whine about 
 * _exit returning.  It really doesn't return... */
#define __NR__exit __NR_exit
_syscall1(void, _exit, int, status);
#endif

//#define __NR_fork             2
#ifdef L___libc_fork
#include <unistd.h>
#	ifdef __UCLIBC_HAS_MMU__
#define __NR___libc_fork __NR_fork
		_syscall0(pid_t, __libc_fork);
		weak_alias (__libc_fork, fork)
#	else
		pid_t fork(void)
		{
			__set_errno(ENOSYS);
			return -1;
		}
#	endif
#endif

//#define __NR_read             3
#ifdef L___libc_read
#include <unistd.h>
#define __NR___libc_read __NR_read
_syscall3(ssize_t, __libc_read, int, fd, __ptr_t, buf, size_t, count);
weak_alias(__libc_read, read)
#endif

//#define __NR_write            4
#ifdef L___libc_write
#include <unistd.h>
#define __NR___libc_write __NR_write
_syscall3(ssize_t, __libc_write, int, fd, const __ptr_t, buf, size_t, count);
weak_alias(__libc_write, write)
/* Stupid libgcc.a from gcc 2.95.x uses __write in pure.o
 * which is a blatent GNU libc-ism... */
weak_alias (__libc_write, __write)
#endif

//#define __NR_open             5
#ifdef L___libc_open
#include <stdarg.h>
/* Do not include fcntl.h, so gcc doesn't whine the prototype */
#define __NR___libc_open __NR_open
_syscall3(int, __libc_open, const char *, fn, int, flags, mode_t, mode);
weak_alias(__libc_open, open)
#endif

//#define __NR_close            6
#ifdef L___libc_close
#include <unistd.h>
#define __NR___libc_close __NR_close
_syscall1(int, __libc_close, int, fd);
weak_alias(__libc_close, close)
#endif

//#define __NR_waitpid          7
// Implemented using wait4 

//#define __NR_creat            8
#ifdef L_creat
#include <fcntl.h>
#ifdef __NR_creat
_syscall2(int, creat, const char *, file, mode_t, mode);
#else
extern int __libc_open (const char *file, int flags, mode_t mode);
int creat (const char *file, mode_t mode)
{
	  return __libc_open (file, O_WRONLY|O_CREAT|O_TRUNC, mode);
}
#endif
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
#include <sys/time.h>
#ifdef __NR_time
_syscall1(time_t, time, time_t *, t);
#else
time_t time (time_t *t)
{ 
	time_t result;
	struct timeval tv;
	if (gettimeofday (&tv, (struct timezone *) NULL)) {
		result = (time_t) -1;
	} else { result = (time_t) tv.tv_sec; }
	if (t != NULL) { *t = result; }
	return result;
}
#endif
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
#ifdef L___libc_lseek
#include <unistd.h>
#define __NR___libc_lseek __NR_lseek
_syscall3(__off_t, __libc_lseek, int, fildes, __off_t, offset, int, whence);
weak_alias(__libc_lseek, lseek)
#ifndef __NR__llseek
weak_alias(__libc_lseek, llseek)
weak_alias(__libc_lseek, lseek64)
#endif
#endif

//#define __NR_getpid           20
#ifdef L___libc_getpid
#include <unistd.h>
#if defined (__alpha__)
#define __NR_getpid     __NR_getxpid
#endif
#define __NR___libc_getpid __NR_getpid
_syscall0(pid_t, __libc_getpid);
weak_alias(__libc_getpid, getpid)
weak_alias(__libc_getpid, __getpid)
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
#ifdef L___setuid
#define __NR___setuid __NR_setuid
#include <unistd.h>
static inline 
_syscall1(int, __setuid, uid_t, uid);
int setuid(uid_t uid)
{
	if (uid == (uid_t) ~0) {
		__set_errno (EINVAL);
		return -1;
	}
	return(__setuid(uid));
}
#endif

//#define __NR_getuid           24
#ifdef L_getuid
#include <unistd.h>
#if defined (__alpha__)
#define __NR_getuid     __NR_getxuid
#endif
_syscall0(uid_t, getuid);
#endif

//#define __NR_stime            25
#ifdef L_stime
#include <time.h>
#include <sys/time.h>
#ifdef _NR_stime
_syscall1(int, stime, const time_t *, t);
#else
int stime(const time_t *when)
{ 
	struct timeval tv;
	if (when == NULL) { __set_errno (EINVAL); return -1; }
	tv.tv_sec = *when;
	tv.tv_usec = 0;
	return settimeofday (&tv, (struct timezone *) 0);
}
#endif
#endif

//#define __NR_ptrace           26
#ifdef L___ptrace
#include <sys/ptrace.h>
#define __NR___ptrace __NR_ptrace
_syscall4(long, __ptrace, enum __ptrace_request, request, pid_t, pid,
		void*, addr, void*, data);
#endif


//#define __NR_alarm            27
#ifdef L_alarm
#include <unistd.h>
#ifdef __NR_alarm
_syscall1(unsigned int, alarm, unsigned int, seconds);
#else
#include <sys/time.h>
unsigned int alarm (unsigned int seconds)
{
	struct itimerval old, new;
	unsigned int retval;
	new.it_value.tv_usec = 0;
	new.it_interval.tv_sec = 0;
	new.it_interval.tv_usec = 0;
	new.it_value.tv_sec = (long int) seconds;
	if (setitimer (ITIMER_REAL, &new, &old) < 0) { return 0; }
	retval = old.it_value.tv_sec;
	if (old.it_value.tv_usec) { ++retval; }
	return retval;
}
#endif
#endif

//#define __NR_oldfstat         28

//#define __NR_pause            29
#ifdef L___libc_pause
#include <unistd.h>
#ifdef __NR_pause
#define __NR___libc_pause __NR_pause
_syscall0(int, __libc_pause);
weak_alias(__libc_pause, pause)
#else
#include <signal.h>
int __libc_pause (void)
{
	return(__sigpause(sigblock(0), 0));
}
weak_alias(__libc_pause, pause)
#endif
#endif

//#define __NR_utime            30
#ifdef L_utime
#include <utime.h>
#ifdef __NR_utime
_syscall2(int, utime, const char *, file, const struct utimbuf *, times);
#else
#include <stdlib.h>
#include <sys/time.h>
int utime(const char *file, const struct utimbuf *times)
{
	struct timeval timevals[2];
	if (times != NULL) {
		timevals[0].tv_usec = 0L;
		timevals[1].tv_usec = 0L;
		timevals[0].tv_sec = (long int) times->actime;
		timevals[1].tv_sec = (long int) times->modtime;
	} else {
		if (gettimeofday (&timevals[0], NULL) < 0) { return -1; }
		timevals[1] = timevals[0];
	}
	return utimes(file, timevals);
}
#endif
#endif

//#define __NR_utimed
#ifdef L_utimes
#include <utime.h>
#ifdef __NR_utimes
_syscall2(int, utimes, const char *, file, const struct timeval *, tvp);
#else
#include <stdlib.h>
#include <sys/time.h>
int utimes (const char *file, const struct timeval tvp[2])
{
	struct utimbuf buf, *times;
	if (tvp) {
		times = &buf;
		times->actime = tvp[0].tv_sec;
		times->modtime = tvp[1].tv_sec;
	} else { times = NULL; }
	return utime(file, times);
}
#endif
#endif

//#define __NR_stty             31
#ifdef L_stty
#include <sgtty.h>
int stty (int __fd, __const struct sgttyb *__params);
{
	__set_errno(ENOSYS);
	return -1;
}
#endif

//#define __NR_gtty             32
#ifdef L_gtty
#include <sgtty.h>
int gtty (int __fd, struct sgttyb *__params)
{
	__set_errno(ENOSYS);
	return -1;
}
#endif

//#define __NR_access           33
#ifdef L_access
#include <unistd.h>
_syscall2(int, access, const char *, pathname, int, mode);
#endif

//#define __NR_nice             34
#ifdef L_nice
#include <unistd.h>
#ifdef __NR_nice
_syscall1(int, nice, int, inc);
#else
#include <sys/resource.h>
int nice (int incr)
{
	int save, prio, result;
	save = errno;
	__set_errno (0);
	prio = getpriority (PRIO_PROCESS, 0);
	if (prio == -1) {
		if (errno != 0) { return -1; } 
		else { __set_errno (save); }
	}
	result = setpriority (PRIO_PROCESS, 0, prio + incr);
	if (result != -1) { return prio + incr; } else { return -1; }
}
#endif
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
_syscall1(int, pipe, int *, filedes);
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
#if defined (__alpha__)
#define __NR_getgid     __NR_getxgid
#endif
_syscall0(gid_t, getgid);
#endif

//#define __NR_signal           48

//#define __NR_geteuid          49
#ifdef	L_geteuid
#include <unistd.h>
#	ifdef	__NR_geteuid
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
#include <unistd.h>
#	ifdef	__NR_getegid
	_syscall0(gid_t, getegid);
#	else
	gid_t getegid(void)
	{
		return (getgid());
	}
#	endif
#endif

//#define __NR_acct             51
#ifdef L_acct
#include <unistd.h>
_syscall1(int, acct, const char *, filename);
#endif

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
extern int _fcntl(int fd, int cmd, long arg);

_syscall3(int, _fcntl, int, fd, int, cmd, long, arg);

int __libc_fcntl(int fd, int command, ...)
{
	long arg;
	va_list list;

	va_start(list, command);
	arg = va_arg(list, long);

	va_end(list);
	return _fcntl(fd, command, arg);
}
weak_alias(__libc_fcntl, fcntl)
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
#ifndef __NR_rt_sigaction
#define __NR___sigaction __NR_sigaction
#ifdef L___sigaction
#include <signal.h>
#undef sigaction
_syscall3(int, __sigaction, int, signum, const struct sigaction *, act,
		  struct sigaction *, oldact);
#endif
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
#ifndef __NR_rt_sigsuspend
#define __NR___sigsuspend __NR_sigsuspend
#ifdef L___sigsuspend
#include <signal.h>
#undef sigsuspend
_syscall3(int, __sigsuspend, int, a, unsigned long int, b, unsigned long int, c);

int sigsuspend (const sigset_t *set)
{
	return __sigsuspend(0, 0, set->__val[0]);
}
#endif
#endif

//#define __NR_sigpending       73
#ifndef __NR_rt_sigpending
#ifdef L_sigpending
#include <signal.h>
#undef sigpending
_syscall1(int, sigpending, sigset_t *, set);
#endif
#endif

//#define __NR_sethostname      74
#ifdef L_sethostname
#include <unistd.h>
_syscall2(int, sethostname, const char *, name, size_t, len);
#endif

//#define __NR_setrlimit        75
#ifndef __NR_ugetrlimit
/* Only wrap setrlimit if the new ugetrlimit is not present */ 
#ifdef L___setrlimit
#define __NR___setrlimit __NR_setrlimit
#include <unistd.h>
#include <sys/resource.h>
#define RMIN(x, y) ((x) < (y) ? (x) : (y))
_syscall2(int, __setrlimit, int, resource, const struct rlimit *, rlim);
int setrlimit (__rlimit_resource_t resource, const struct rlimit *rlimits)
{
	struct rlimit rlimits_small;
	/* We might have to correct the limits values.  Since the old values
	 * were signed the new values might be too large.  */
	rlimits_small.rlim_cur = RMIN ((unsigned long int) rlimits->rlim_cur,
				       RLIM_INFINITY >> 1);
	rlimits_small.rlim_max = RMIN ((unsigned long int) rlimits->rlim_max,
				       RLIM_INFINITY >> 1);
	return(__setrlimit(resource, &rlimits_small));
}
#undef RMIN
#endif
#else /* We don't need to wrap setrlimit */
#ifdef L_setrlimit
#include <unistd.h>
#include <sys/resource.h>
_syscall2(int, setrlimit, int, resource, const struct rlimit *, rlim);
#endif
#endif /* __NR_setrlimit */

//#define __NR_getrlimit        76
#ifdef L___getrlimit
/* Only include the old getrlimit if the new one (ugetrlimit) is not around */ 
#ifndef __NR_ugetrlimit
#define __NR___getrlimit __NR_getrlimit
#include <unistd.h>
#include <sys/resource.h>
_syscall2(int, __getrlimit, int, resource, struct rlimit *, rlim);
int getrlimit (__rlimit_resource_t resource, struct rlimit *rlimits)
{
	int result;
	result = __getrlimit(resource, rlimits);

	if (result == -1)
		return result;

	/* We might have to correct the limits values.  Since the old values
	 * were signed the infinity value is too small.  */
	if (rlimits->rlim_cur == RLIM_INFINITY >> 1)
		rlimits->rlim_cur = RLIM_INFINITY;
	if (rlimits->rlim_max == RLIM_INFINITY >> 1)
		rlimits->rlim_max = RLIM_INFINITY;
	return result;
}
#endif
#endif /* __NR_getrlimit */

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
#ifdef L_select
//Used as a fallback if _newselect isn't available...
#ifndef __NR__newselect
#include <unistd.h>
extern int select(int n, fd_set *readfds, fd_set *writefds, 
		fd_set *exceptfds, struct timeval *timeout);
_syscall5(int, select, int, n, fd_set *, readfds, fd_set *, writefds,
		fd_set *, exceptfds, struct timeval *, timeout);
#endif
#endif

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
extern int _reboot(int magic, int magic2, int flag);

_syscall3(int, _reboot, int, magic, int, magic2, int, flag);

int reboot(int flag)
{
	return (_reboot((int) 0xfee1dead, 672274793, flag));
}
#endif

//#define __NR_readdir          89

//#define __NR_mmap             90
#ifdef L__mmap
#define __NR__mmap __NR_mmap
#include <unistd.h>
#include <sys/mman.h>
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
_syscall2(int, truncate, const char *, path, __off_t, length);
#endif

//#define __NR_ftruncate        93
#ifdef L_ftruncate
#include <unistd.h>
_syscall2(int, ftruncate, int, fd, __off_t, length);
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
#ifdef __NR_socketcall
_syscall2(int, socketcall, int, call, unsigned long *, args);
#endif
#endif

//#define __NR_syslog           103
#ifdef L__syslog
#include <unistd.h>
#define __NR__syslog		__NR_syslog
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
#if ! defined __NR_stat64 && defined __UCLIBC_HAVE_LFS__
weak_alias(stat, stat64);
weak_alias(__xstat, __xstat64);
#endif
#endif

//#define __NR_lstat            107
#ifdef L___lstat
#include <unistd.h>
#include "statfix.h"
#define __NR___lstat	__NR_lstat
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
#if ! defined __NR_lstat64 && defined __UCLIBC_HAVE_LFS__
weak_alias(lstat, lstat64);
weak_alias(__lxstat, __lxstat64);
#endif
#endif

//#define __NR_fstat            108
#ifdef L___fstat
#include <unistd.h>
#include "statfix.h"
#define __NR___fstat	__NR_fstat
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
#if ! defined __NR_fstat64 && defined __UCLIBC_HAVE_LFS__
weak_alias(fstat, fstat64);
weak_alias(__fxstat, __fxstat64);
#endif
#endif

//#define __NR_olduname         109

//#define __NR_iopl             110
#ifdef L_iopl
/* Tuns out the m68k unistd.h kernel header is broken */
#	if defined __UCLIBC_HAS_MMU__ && defined __NR_iopl && ! defined(__mc68000__)
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
#ifdef __NR_ipc
#define __NR___ipc __NR_ipc
_syscall5(int, __ipc, unsigned int, call, int, first, int, second, int, third, void *, ptr);
#endif
#endif

//#define __NR_fsync            118
#ifdef L___libc_fsync
#include <unistd.h>
#define __NR___libc_fsync __NR_fsync
_syscall1(int, __libc_fsync, int, fd);
weak_alias(__libc_fsync, fsync)
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
weak_alias(adjtimex, __adjtimex);
#endif

//#define __NR_mprotect         125
#ifdef L_mprotect
#include <sys/mman.h>
_syscall3(int, mprotect, void *, addr, size_t, len, int, prot);
#endif

//#define __NR_sigprocmask      126
#ifndef __NR_rt_sigprocmask
#ifdef L_sigprocmask
#include <signal.h>
#undef sigprocmask
_syscall3(int, sigprocmask, int, how, const sigset_t *, set, 
		sigset_t *, oldset);
#endif
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
#ifdef L_get_kernel_syms
struct kernel_sym;
_syscall1(int, get_kernel_syms, struct kernel_sym *, table);
#endif

//#define __NR_quotactl         131
#ifdef __NR_quotactl
#ifdef L_quotactl
#include <sys/quota.h>
_syscall4(int, quotactl, int, cmd, const char *, special , int, id, caddr_t, addr);
#endif
#endif

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
#ifdef __NR_setfsuid
#ifdef L_setfsuid
#include <sys/fsuid.h>
_syscall1(int, setfsuid, uid_t, uid);
#endif
#endif

//#define __NR_setfsgid         139
#ifdef __NR_setfsgid
#ifdef L_setfsgid
#include <sys/fsuid.h>
_syscall1(int, setfsgid, gid_t, gid);
#endif
#endif

//#define __NR__llseek          140
#ifdef L__llseek
#ifdef __UCLIBC_HAVE_LFS__
#ifdef __NR__llseek
extern int _llseek(int fd, __off_t offset_hi, __off_t offset_lo, 
		__loff_t *result, int whence);

_syscall5(int, _llseek, int, fd, __off_t, offset_hi, __off_t, offset_lo, 
		__loff_t *, result, int, whence);

__loff_t __libc_lseek64(int fd, __loff_t offset, int whence)
{
	int ret;
	__loff_t result;

	ret = _llseek(fd, (__off_t) (offset >> 32),
				  (__off_t) (offset & 0xffffffff), &result, whence);

	return ret ? (__loff_t) ret : result;
}
weak_alias(__libc_lseek64, llseek);
weak_alias(__libc_lseek64, lseek64);
#endif
#endif
#endif

//#define __NR_getdents         141
#ifdef L_getdents
#include <unistd.h>
#include <dirent.h>
_syscall3(int, getdents, int, fd, char *, dirp, size_t, count);
#endif

//#define __NR__newselect       142
#ifdef L__newselect
//Used in preference to select when available...
#ifdef __NR__newselect
#include <unistd.h>
extern int _newselect(int n, fd_set *readfds, fd_set *writefds,
					  fd_set *exceptfds, struct timeval *timeout);
_syscall5(int, _newselect, int, n, fd_set *, readfds, fd_set *, writefds,
		fd_set *, exceptfds, struct timeval *, timeout);
weak_alias(_newselect, select);
#endif
#endif

//#define __NR_flock            143
#ifdef L_flock
#include <sys/file.h>
_syscall2(int,flock,int,fd, int,operation);
#endif

//#define __NR_msync            144
#ifdef L___libc_msync
#include <unistd.h>
#include <sys/mman.h>
#define __NR___libc_msync __NR_msync
_syscall3(int, __libc_msync, void *, addr, size_t, length, int, flags);
weak_alias(__libc_msync, msync);
#endif

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
#ifdef __NR_fdatasync
#ifdef L_fdatasync
#include <unistd.h>
_syscall1(int, fdatasync, int, fd);
#endif
#endif

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
#ifdef __NR_sched_setparam
#ifdef L_sched_setparam
#include <sched.h>
_syscall2(int, sched_setparam, pid_t, pid, const struct sched_param *, p);
#endif
#endif

//#define __NR_sched_getparam   155
#ifdef __NR_sched_getparam
#ifdef L_sched_getparam
#include <sched.h>
_syscall2(int, sched_getparam, pid_t, pid, struct sched_param *, p);
#endif
#endif

//#define __NR_sched_setscheduler       156
#ifdef __NR_sched_setscheduler
#ifdef L_sched_setscheduler
#include <sched.h>
_syscall3(int, sched_setscheduler, pid_t, pid, int, policy, const struct sched_param *, p);
#endif
#endif

//#define __NR_sched_getscheduler       157
#ifdef __NR_sched_getscheduler
#ifdef L_sched_getscheduler
#include <sched.h>
_syscall1(int, sched_getscheduler, pid_t, pid);
#endif
#endif

//#define __NR_sched_yield              158
#ifdef __NR_sched_yield
#ifdef L_sched_yield
#include <sched.h>
_syscall0(int, sched_yield);
#endif
#endif

//#define __NR_sched_get_priority_max   159
#ifdef __NR_sched_get_priority_max
#ifdef L_sched_get_priority_max
#include <sched.h>
_syscall1(int, sched_get_priority_max, int, policy);
#endif
#endif

//#define __NR_sched_get_priority_min   160
#ifdef __NR_sched_get_priority_min
#ifdef L_sched_get_priority_min
#include <sched.h>
_syscall1(int, sched_get_priority_min, int, policy);
#endif
#endif

//#define __NR_sched_rr_get_interval    161
#ifdef __NR_sched_rr_get_interval
#ifdef L_sched_rr_get_interval
#include <sched.h>
_syscall2(int, sched_rr_get_interval, pid_t, pid, struct timespec *, tp);
#endif
#endif

//#define __NR_nanosleep                162
#ifdef L___libc_nanosleep
#include <time.h>
#define __NR___libc_nanosleep __NR_nanosleep
_syscall2(int, __libc_nanosleep, const struct timespec *, req, struct timespec *, rem);
weak_alias(__libc_nanosleep, nanosleep)
#endif

//#define __NR_mremap                   163
#ifdef L_mremap
#include <unistd.h>
#include <sys/mman.h>
_syscall4(__ptr_t, mremap, __ptr_t, old_address, size_t, old_size, size_t, new_size, int, may_move);
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
//nfsservctl	EXTRA	nfsservctl	i:ipp	nfsservctl

//#define __NR_setresgid                170
//#define __NR_getresgid                171
//#define __NR_prctl                    172

//#define __NR_rt_sigreturn             173
//#define __NR_rt_sigaction             174
#ifdef __NR_rt_sigaction
#define __NR___rt_sigaction __NR_rt_sigaction
#ifdef L___rt_sigaction
#include <signal.h>
#undef sigaction
_syscall4(int, __rt_sigaction, int, signum, const struct sigaction *, act, 
		struct sigaction *, oldact, size_t, size); 
#endif
#endif

//#define __NR_rt_sigprocmask           175
#ifdef __NR_rt_sigprocmask
#define __NR___rt_sigprocmask __NR_rt_sigprocmask
#ifdef L___rt_sigprocmask
#include <signal.h>
#undef sigprocmask
_syscall4(int, __rt_sigprocmask, int, how, const sigset_t *, set, 
		sigset_t *, oldset, size_t, size);

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset) 
{
	  return __rt_sigprocmask(how, set, oldset, _NSIG/8);
}
#endif
#endif

//#define __NR_rt_sigpending            176
#ifdef __NR_rt_sigpending
#define __NR___rt_sigpending __NR_rt_sigpending
#ifdef L___rt_sigpending
#include <signal.h>
#undef sigpending
_syscall2(int, __rt_sigpending, sigset_t *, set, size_t, size);

int sigpending(sigset_t *set) 
{
	return __rt_sigpending(set, _NSIG/8);
}
#endif
#endif

//#define __NR_rt_sigtimedwait          177
#ifdef L___rt_sigtimedwait
#include <signal.h>
#define __need_NULL
#include <stddef.h>
#ifdef __NR_rt_sigtimedwait
#define __NR___rt_sigtimedwait __NR_rt_sigtimedwait
_syscall4(int, __rt_sigtimedwait, const sigset_t *, set, siginfo_t *, info, 
		const struct timespec *, timeout, size_t, setsize);

int sigwaitinfo(const sigset_t *set, siginfo_t *info)
{
	return __rt_sigtimedwait (set, info, NULL, _NSIG/8);
}

int sigtimedwait (const sigset_t *set, siginfo_t *info, const struct timespec *timeout)
{
	return __rt_sigtimedwait (set, info, timeout, _NSIG/8);
}
#else
int sigwaitinfo(const sigset_t *set, siginfo_t *info)
{
	if (set==NULL)
		__set_errno (EINVAL);
	else
		__set_errno (ENOSYS);
	return -1;
}

int sigtimedwait (const sigset_t *set, siginfo_t *info, const struct timespec *timeout)
{
	if (set==NULL)
		__set_errno (EINVAL);
	else
		__set_errno (ENOSYS);
	return -1;
}
#endif
#endif

//#define __NR_rt_sigqueueinfo          178

//#define __NR_rt_sigsuspend            179
#ifdef __NR_rt_sigsuspend
#define __NR___rt_sigsuspend __NR_rt_sigsuspend
#ifdef L___rt_sigsuspend
#include <signal.h>
#undef _sigsuspend
_syscall2(int, __rt_sigsuspend, const sigset_t *, mask, size_t, size);

int sigsuspend (const sigset_t *mask)
{
	return __rt_sigsuspend(mask, _NSIG/8);
}
#endif
#endif

//#define __NR_pread                    180
#ifdef L___libc_pread
#ifdef __NR_pread
#define _XOPEN_SOURCE 500
#include <unistd.h>
#define __NR___libc_pread __NR_pread
_syscall4(ssize_t, __libc_pread, int, fd, void *, buf, size_t, count, __off_t, offset);
weak_alias (__libc_pread, pread)
#else
ssize_t pread(int fd, void *buf, size_t count, off_t offset)                                                
{                                                                                                           
	__set_errno(ENOSYS);                                                                                    
	return -1;                                                                                              
}                                                                                                           
#endif
#endif

//#define __NR_pwrite                   181
#ifdef L___libc_pwrite
#ifdef __NR_pwrite
#define _XOPEN_SOURCE 500
#include <unistd.h>
#define __NR___libc_pwrite __NR_pwrite
_syscall4(ssize_t, __libc_pwrite, int, fd, const void *, buf, size_t, count, __off_t, offset);
weak_alias (__libc_pwrite, pwrite)
#else
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset)                                         
{                                                                                                           
	__set_errno(ENOSYS);                                                                                    
	return -1;                                                                                              
}                                                                                                           
#endif
#endif

//#define __NR_chown                    182
#ifdef L_chown
#include <unistd.h>
_syscall3(int, chown, const char *, path, uid_t, owner, gid_t, group);
#endif

//#define __NR_getcwd                   183
// See getcwd.c in this directory

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
#ifdef __NR_sigaltstack
#ifdef L_sigaltstack
#include <signal.h>
_syscall2(int, sigaltstack, const struct sigaltstack *, ss, struct sigaltstack *, oss);
#endif
#endif

//#define __NR_sendfile                 187
#ifdef __NR_sendfile
#ifdef L_sendfile
#include <unistd.h>
#include <sys/sendfile.h>
_syscall4(ssize_t,sendfile, int, out_fd, int, in_fd, __off_t *, offset, size_t, count)
#endif
#endif

//#define __NR_getpmsg                  188

//#define __NR_putpmsg                  189

//#define __NR_vfork                    190
//See sysdeps/linux/<arch>vfork.[cS] for architecture specific implementation...

//#define __NR_ugetrlimit		191	/* SuS compliant getrlimit */
#ifdef L___ugetrlimit
#ifdef __NR_ugetrlimit
#define __NR___ugetrlimit __NR_ugetrlimit
#include <unistd.h>
#include <sys/resource.h>
_syscall2(int, __ugetrlimit, enum __rlimit_resource, resource, struct rlimit *, rlim);
int getrlimit (__rlimit_resource_t resource, struct rlimit *rlimits)
{
	return(__ugetrlimit(resource, rlimits));
}
#endif /* __NR_ugetrlimit */
#endif


//#define __NR_mmap2		192


//#define __NR_truncate64         193
//See libc/sysdeps/linux/common/truncate64.c

//#define __NR_ftruncate64        194
//See libc/sysdeps/linux/common/ftruncate64.c


//#define __NR_stat64             195
#ifdef L___stat64
#if defined __NR_stat64 && defined __UCLIBC_HAVE_LFS__
#include <unistd.h>
#include "statfix64.h"
#define __NR___stat64	__NR_stat64
extern int __stat64(const char *file_name, struct kernel_stat64 *buf);
_syscall2(int, __stat64, const char *, file_name, struct kernel_stat64 *, buf);

int __xstat64(int version, const char * file_name, struct libc_stat64 * cstat)
{
	struct kernel_stat64 kstat;
	int result = __stat64(file_name, &kstat);

	if (result == 0) { 
		statfix64(cstat, &kstat);
	}
	return result;
}

int stat64(const char *file_name, struct libc_stat64 *buf)
{
	return(__xstat64(0, file_name, buf));
}
#endif /* __UCLIBC_HAVE_LFS__ */
#endif

//#define __NR_lstat64            196
#ifdef L___lstat64
#if defined __NR_lstat64 && defined __UCLIBC_HAVE_LFS__
#include <unistd.h>
#include "statfix64.h"
#define __NR___lstat64	__NR_lstat64
extern int __lstat64(const char *file_name, struct kernel_stat64 *buf);
_syscall2(int, __lstat64, const char *, file_name, struct kernel_stat64 *, buf);

int __lxstat64(int version, const char * file_name, struct libc_stat64 * cstat)
{
	struct kernel_stat64 kstat;
	int result = __lstat64(file_name, &kstat);

	if (result == 0) { 
		statfix64(cstat, &kstat);
	}
	return result;
}

int lstat64(const char *file_name, struct libc_stat64 *buf)
{
	return(__lxstat64(0, file_name, buf));
}
#endif /* __UCLIBC_HAVE_LFS__ */
#endif

//#define __NR_fstat64            197
#ifdef L___fstat64
#if defined __NR_fstat64 && defined __UCLIBC_HAVE_LFS__
#include <unistd.h>
#include "statfix64.h"
#define __NR___fstat64	__NR_fstat64
extern int __fstat64(int filedes, struct kernel_stat64 *buf);
_syscall2(int, __fstat64, int, filedes, struct kernel_stat64 *, buf);

int __fxstat64(int version, int fd, struct libc_stat64 * cstat)
{
	struct kernel_stat64 kstat;
	int result = __fstat64(fd, &kstat);

	if (result == 0) { 
		statfix64(cstat, &kstat);
	}
	return result;
}

int fstat64(int filedes, struct libc_stat64 *buf)
{
	return(__fxstat64(0, filedes, buf));
}
#endif /* __UCLIBC_HAVE_LFS__ */
#endif


//#define __NR_lchown32		198
//#define __NR_getuid32		199
//#define __NR_getgid32		200
//#define __NR_geteuid32		201
//#define __NR_getegid32		202
//#define __NR_setreuid32		203
//#define __NR_setregid32		204
//#define __NR_getgroups32	205
//#define __NR_setgroups32	206
//#define __NR_fchown32		207
//#define __NR_setresuid32	208
//#define __NR_getresuid32	209
//#define __NR_setresgid32	210
//#define __NR_getresgid32	211
//#define __NR_chown32		212
//#define __NR_setuid32		213
//#define __NR_setgid32		214
//#define __NR_setfsuid32		215
//#define __NR_setfsgid32		216
//#define __NR_pivot_root		217
#ifdef __NR_pivot_root
#ifdef L_pivot_root
_syscall2(int, pivot_root, const char *, new_root, const char *, put_old)
#endif
#endif

//#define __NR_mincore		218
//#define __NR_madvise		219
//#define __NR_madvise1		219	/* delete when C lib stub is removed */

//#define __NR_getdents64		220
#ifdef L_getdents64
#ifdef __UCLIBC_HAVE_LFS__
#include <unistd.h>
#include <dirent.h>
_syscall3(int, getdents64, int, fd, char *, dirp, size_t, count);
#endif /* __UCLIBC_HAVE_LFS__ */
#endif

//#define __NR_fcntl64		221
#ifdef L__fcntl64
#ifdef __UCLIBC_HAVE_LFS__
#ifdef __NR_fcntl64
#define __NR__fcntl64 __NR_fcntl64
#include <stdarg.h>
#include <fcntl.h>
extern int _fcntl64(int fd, int cmd, long arg);

_syscall3(int, _fcntl64, int, fd, int, cmd, long, arg);

int fcntl64(int fd, int command, ...)
{
	long arg;
	va_list list;

	va_start(list, command);
	arg = va_arg(list, long);

	va_end(list);
	return _fcntl64(fd, command, arg);
}
#else
extern int __libc_fcntl(int fd, int command, ...);
weak_alias(__libc_fcntl, fcntl64)
#endif
#endif
#endif

//#define __NR_security		223	/* syscall for security modules */
//#define __NR_gettid		224
//#define __NR_readahead		225

