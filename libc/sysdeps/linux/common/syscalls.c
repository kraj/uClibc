/* vi: set sw=4 ts=4: */
/*
 * Syscalls for uC-Libc
 *
 * Copyright (C) 2000 by Lineo, inc.  Written by Erik Andersen
 * <andersen@lineo.com>, <andersee@debian.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
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
_syscall1(void, _exit, int, status);
#endif

//#define __NR_fork             2
#ifdef L_fork
#include <unistd.h>
_syscall0(pid_t, fork);
#endif

//#define __NR_read             3
#ifdef L_read
#include <unistd.h>
_syscall3(size_t, read, int, fd, char *, buf, size_t, count);
#endif

//#define __NR_write            4
#ifdef L_write
#include <unistd.h>
_syscall3(size_t, write, int, fd, const char *, buf, size_t, count);
#endif

//#define __NR_open             5
#ifdef L___open
#include <stdarg.h>
#include <fcntl.h>
#define __NR___open __NR_open
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
#include <sys/wait.h>
#ifdef L_waitpid
_syscall3(pid_t, waitpid, pid_t, pid, int *, status, int, options);
#endif

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
_syscall3(int, mknod, const char *, pathname, mode_t, mode, dev_t, dev);
#endif

//#define __NR_chmod            15
#ifdef L_chmod
#include <sys/stat.h>
_syscall2(int, chmod, const char *, path, mode_t, mode);
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
_syscall1(int, stime, time_t *, t);
#endif

//#define __NR_ptrace           26

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
_syscall2(int, utime, const char *, filename, struct utimbuf *, buf);
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
_syscall0(int, sync);
#endif

//#define __NR_kill             37
#ifdef L_kill
#include <signal.h>
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
_syscall0(gid_t, getgid);
#endif

//#define __NR_signal           48

//#define __NR_geteuid          49
#ifdef	L_geteuid
#ifdef	SYS_geteuid
#include <unistd.h>
_syscall0(uid_t, geteuid);
#else
uid_t geteuid(void)
{
	return (getuid());
}
#endif
#endif

//#define __NR_getegid          50
#ifdef	L_getegid
#ifdef	SYS_getegid
#include <unistd.h>
_syscall0(gid_t, getegid);
#else
gid_t getegid(void)
{
	return (getgid());
}
#endif
#endif

//#define __NR_acct             51

//#define __NR_umount2          52
#ifdef L_umount2
#include <sys/mount.h>
_syscall2(int, umount2, const char *, special_file, int, flags);
#endif

//#define __NR_lock             53

//#define __NR_ioctl            54
#ifdef L__ioctl
#include <stdarg.h>
#include <sys/ioctl.h>
#define __NR__ioctl __NR_ioctl
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
#include <unistd.h>
#ifdef	SYS_getppid
_syscall0(pid_t, getppid);
#else
pid_t getppid(void)
{
	return (getpid());
}
#endif
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
_syscall1(int, sigsuspend, const sigset_t *, mask);
#endif

//#define __NR_sigpending       73
#ifdef L_sigpending
#include <signal.h>
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
_syscall2(int, getrusage, int, who, struct rusage *, usage);
#endif

//#define __NR_gettimeofday     78
#ifdef L_gettimeofday
#include <unistd.h>
_syscall2(int, gettimeofday, struct timeval *, tv, struct timezone *, tz);
#endif

//#define __NR_settimeofday     79
#ifdef L_settimeofday
#include <unistd.h>
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
_syscall2(int, getpriority, int, which, int, who);
#endif

//#define __NR_setpriority      97
#ifdef L_setpriority
#include <sys/resource.h>
_syscall3(int, setpriority, int, which, int, who, int, prio);
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
syscall3(int, ioperm, unsigned, long, from, unsigned long, num, int,

		 turn_on);
#endif

//#define __NR_socketcall       102
#ifdef L_socketcall
_syscall2(int, socketcall, int, call, unsigned long *, args);
#endif

//#define __NR_syslog           103
#ifdef L__syslog
#include <unistd.h>
#define __NR__syslog		__NR_syslog
_syscall3(int, _syslog, int, type, char *, buf, int, len);

int klogctl(int type, char *buf, int len)
{
	return (_syslog(type, buf, len));
}

#endif

//#define __NR_setitimer        104
#ifdef L_setitimer
#include <sys/time.h>
_syscall3(int, setitimer, enum __itimer_which, which,
		  const struct itimerval *, new, struct itimerval *, old);
#endif

//#define __NR_getitimer        105
#ifdef L_getitimer
#include <sys/time.h>
_syscall2(int, getitimer, enum __itimer_which, which, struct itimerval *, value);
#endif

//#define __NR_stat             106
#ifdef L__stat
#define __NR__stat __NR_stat
#include <unistd.h>
#include "statfix.h"
_syscall2(int, _stat, const char *, file_name, struct kernel_stat *, buf);

int stat(const char * file_name, struct libc_stat * cstat)
{
	struct kernel_stat kstat;
	int result = _stat(file_name, &kstat);

	if (result == 0) { 
		statfix(cstat, &kstat);
	}
	return result;
}
#endif

//#define __NR_lstat            107
#ifdef L__lstat
#define __NR__lstat __NR_lstat
#include <unistd.h>
#include "statfix.h"
_syscall2(int, _lstat, const char *, file_name, struct kernel_stat *, buf);

int lstat(const char * file_name, struct libc_stat * cstat)
{
	struct kernel_stat kstat;
	int result = _lstat(file_name, &kstat);

	if (result == 0) { 
		statfix(cstat, &kstat);
	}
	return result;
}
#endif

//#define __NR_fstat            108
#ifdef L__fstat
#define __NR__fstat __NR_fstat
#include <unistd.h>
#include "statfix.h"
_syscall2(int, _fstat, int, filedes, struct kernel_stat *, buf);

int fstat(int fd, struct libc_stat *cstat) 
{
	struct kernel_stat kstat;
	int result = _fstat(fd, &kstat);

	if (result == 0) { 
		statfix(cstat, &kstat);
	}
	return result;
}
#endif

//#define __NR_olduname         109

//#define __NR_iopl             110
#ifdef L_iopl
#include <sys/io.h>
_syscall1(int, iopl, int, level);
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
#include <sys/wait.h>
_syscall4(pid_t, wait4, pid_t, pid, int *, status, int, options,
		  struct rusage *, rusage);
#endif

//#define __NR_swapoff          115
#ifdef L_swapoff
#include <sys/swap.h>
_syscall1(int, swapoff, const char *, path);
#endif

//#define __NR_sysinfo          116
#ifdef L_sysinfo
#include <sys/sysinfo.h>
_sysinfo(int, sysinfo, struct sysinfo *, info);
#endif

//#define __NR_ipc              117
#ifdef L_ipc
//_syscall6(int,ipc,unsigned int,call, int,first, int,second, int,third, void *,ptr, long, fifth);
#endif

//#define __NR_fsync            118
#ifdef L_fsync
#include <unistd.h>
_syscall1(int, fsync, int, fd);
#endif

//#define __NR_sigreturn        119
//int sigreturn(unsigned long __unused);

//#define __NR_clone            120

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
_syscall3(int, mprotect, const void *, addr, size_t, len, int, prot);
#endif

//#define __NR_sigprocmask      126
#ifdef L_sigprocmask
#include <signal.h>
_syscall3(int, sigprocmask, int, how, const sigset_t *, set, sigset_t *,

		  oldset);
#endif

//#define __NR_create_module    127

//#define __NR_init_module      128

//#define __NR_delete_module    129

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

//#define __NR_sysfs            135

//#define __NR_personality      136

//#define __NR_afs_syscall      137

//#define __NR_setfsuid         138
#ifdef L_setfsuid
SYSCALL__(setfsuid, 1)
	ret
#endif
//#define __NR_setfsgid         139
#ifdef L_setfsgid
	SYSCALL__(setfsgid, 1)
	ret
#endif
//#define __NR__llseek          140
#ifdef L__llseek
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
#ifdef L__getdents
#define __NR__getdents __NR_getdents
#include <unistd.h>
#include <dirent.h>
_syscall3(int, _getdents, int, fd, char *, dirp, size_t, count);
#endif

//#define __NR__newselect       142
#ifdef L__newselect
#include <unistd.h>
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
SYSCALL__(flock, 2)
	ret
#endif
//#define __NR_msync            144
#ifdef L_msync
	SYSCALL__(msync, 3)
	ret
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
SYSCALL__(getsid, 1)
	ret
#endif
//#define __NR_fdatasync        148
#ifdef L_fdatasync
	SYSCALL__(fdatasync, 1)
	ret
#endif
//#define __NR__sysctl          149
//#define __NR_mlock            150
#ifdef L_mlock
	SYSCALL__(mlock, 2)
	ret
#endif
//#define __NR_munlock          151
#ifdef L_munlock
	SYSCALL__(munlock, 2)
	ret
#endif
//#define __NR_mlockall         152
#ifdef L_mlockall
	SYSCALL__(mlockall, 1)
	ret
#endif
//#define __NR_munlockall       153
#ifdef L_munlockall
	SYSCALL__(munlockall, 0)
	ret
#endif
//#define __NR_sched_setparam   154
#ifdef L_sched_setparam
	SYSCALL__(sched_setparam, 2)
	ret
#endif
//#define __NR_sched_getparam   155
#ifdef L_sched_getparam
	SYSCALL__(sched_getparam, 2)
	ret
#endif
//#define __NR_sched_setscheduler       156
#ifdef L_sched_setscheduler
	SYSCALL__(sched_setscheduler, 3)
	ret
#endif
//#define __NR_sched_getscheduler       157
#ifdef L_sched_getscheduler
	SYSCALL__(sched_getscheduler, 1)
	ret
#endif
//#define __NR_sched_yield              158
#ifdef L_sched_yield
	SYSCALL__(sched_yield, 0)
	ret
#endif
//#define __NR_sched_get_priority_max   159
#ifdef L_sched_get_priority_max
	SYSCALL__(sched_get_priority_max, 1)
	ret
#endif
//#define __NR_sched_get_priority_min   160
#ifdef L_sched_get_priority_min
	SYSCALL__(sched_get_priority_min, 1)
	ret
#endif
//#define __NR_sched_rr_get_interval    161
#ifdef L_sched_rr_get_interval
	SYSCALL__(sched_rr_get_interval, 2)
	ret
#endif
//#define __NR_nanosleep                162
#ifdef L_nanosleep
	SYSCALL__(nanosleep, 2)
	ret
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

//#define __NR_poll                     168
#ifdef L_poll
SYSCALL__(poll, 3)
	ret
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

//#define __NR_capget                   184

//#define __NR_capset                   185

//#define __NR_sigaltstack              186

//#define __NR_sendfile                 187

//#define __NR_getpmsg                  188

//#define __NR_putpmsg                  189

//#define __NR_vfork                    190
