# Makefile for uClibc
#
# Copyright (C) 2000-2003 Erik Andersen <andersen@uclibc.org>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Library General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for more
# details.
#
# You should have received a copy of the GNU Library General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

TOPDIR=../../../../
include $(TOPDIR)Rules.mak

CSRC=	__rt_sigtimedwait.c __socketcall.c __syscall_fcntl.c \
	__syscall_fcntl64.c __syscall_ipc.c __syscall_rt_sigaction.c \
	__syscall_sigaction.c _exit.c access.c acct.c adjtimex.c alarm.c \
	bdflush.c capget.c capset.c chdir.c chmod.c chown.c chroot.c \
	close.c cmsg_nxthdr.c creat64.c create_module.c delete_module.c \
	dup.c dup2.c epoll.c execve.c fchdir.c fchmod.c fchown.c fdatasync.c \
	flock.c fork.c fstat.c fstat64.c fstatfs.c fsync.c ftruncate.c \
	ftruncate64.c get_kernel_syms.c getcwd.c getdents.c getdents64.c \
	getdirname.c getdnnm.c getdtablesize.c getegid.c geteuid.c \
	getgid.c getgroups.c gethstnm.c getitimer.c getpagesize.c \
	getpgid.c getpgrp.c getpid.c getppid.c getpriority.c getresgid.c \
	getresuid.c getrlimit.c getrlimit64.c getrusage.c getsid.c \
	gettimeofday.c getuid.c init_module.c ioctl.c ioperm.c iopl.c \
	kill.c klogctl.c lchown.c link.c llseek.c longjmp.c lseek.c lstat.c \
	lstat64.c madvise.c mkdir.c mkfifo.c mincore.c mknod.c mlock.c \
	mlockall.c mmap.c mmap64.c modify_ldt.c mount.c mprotect.c \
	mremap.c msync.c munlock.c munlockall.c munmap.c nanosleep.c \
	nice.c noophooks.c ntp_gettime.c open.c open64.c pause.c \
	personality.c pipe.c pivot_root.c poll.c prctl.c pread_write.c \
	ptrace.c query_module.c quotactl.c read.c readlink.c readv.c \
	reboot.c rename.c rmdir.c sched_get_priority_max.c \
	sched_get_priority_min.c sched_getparam.c sched_getscheduler.c \
	sched_rr_get_interval.c sched_setparam.c sched_setscheduler.c \
	sched_yield.c select.c sendfile.c sendfile64.c setdomainname.c \
	setegid.c seteuid.c setfsgid.c setfsuid.c setgid.c setgroups.c \
	sethostname.c setitimer.c setpgid.c setpgrp.c setpriority.c \
	setregid.c setresgid.c setresuid.c setreuid.c setrlimit.c \
	setrlimit64.c setsid.c settimeofday.c setuid.c sigaltstack.c \
	sigpending.c sigprocmask.c sigsuspend.c stat.c stat64.c statfs.c \
	stime.c swapoff.c swapon.c symlink.c sync.c sysctl.c sysfs.c \
	sysinfo.c time.c times.c truncate.c truncate64.c ulimit.c \
	umask.c umount.c umount2.c uname.c unlink.c uselib.c ustat.c \
	utime.c utimes.c vfork.c vhangup.c wait.c wait3.c wait4.c \
	waitpid.c write.c writev.c xattr.c xstatconv.c

ifneq ($(strip $(EXCLUDE_BRK)),y)
CSRC+=sbrk.c
endif
ifeq ($(strip $(UCLIBC_HAS_SSP)),y)
CSRC+=ssp.c
endif

COBJS=$(patsubst %.c,%.o, $(CSRC))

OBJ=$(COBJS) $(MOBJ)

all: $(STR_SYSCALLS) $(OBJ) $(LIBC)

$(LIBC): ar-target

ar-target: $(OBJ)
	$(AR) $(ARFLAGS) $(LIBC) $(OBJ)

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

headers:
	$(LN) -fs ../libc/sysdeps/linux/common/fpu_control.h $(TOPDIR)/include/

clean:
	$(RM) *.[oa] *~ core
