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

CSRC=	waitpid.c getdnnm.c gethstnm.c getcwd.c ptrace.c \
	mkfifo.c setegid.c wait.c getpagesize.c seteuid.c \
	wait3.c setpgrp.c getdtablesize.c create_module.c \
	cmsg_nxthdr.c longjmp.c open64.c ftruncate64.c mmap64.c \
	truncate64.c getrlimit64.c setrlimit64.c creat64.c \
	llseek.c pread_write.c _exit.c sync.c getdirname.c \
	sendfile64.c xstatconv.c getdents.c getdents64.c vfork.c \
	ulimit.c ntp_gettime.c poll.c \
	fork.c read.c write.c open.c \
	close.c link.c unlink.c execve.c chdir.c \
	time.c mknod.c chmod.c lchown.c \
	lseek.c getpid.c mount.c umount.c setuid.c \
	getuid.c stime.c alarm.c pause.c utime.c utimes.c \
	access.c nice.c kill.c rename.c mkdir.c \
	rmdir.c dup.c pipe.c times.c setgid.c getgid.c \
	geteuid.c getegid.c acct.c umount2.c \
	ioctl.c setpgid.c \
	umask.c chroot.c ustat.c dup2.c \
	getppid.c getpgrp.c setsid.c __syscall_sigaction.c \
	setreuid.c setregid.c sigsuspend.c sigpending.c \
	sethostname.c setrlimit.c getrlimit.c getrusage.c \
	gettimeofday.c settimeofday.c getgroups.c \
	setgroups.c select.c symlink.c readlink.c uselib.c swapon.c \
	reboot.c mmap.c munmap.c truncate.c ftruncate.c fchmod.c \
	fchown.c getpriority.c setpriority.c \
	statfs.c fstatfs.c ioperm.c __socketcall.c klogctl.c \
	setitimer.c getitimer.c stat.c lstat.c \
	fstat.c iopl.c vhangup.c wait4.c swapoff.c \
	sysinfo.c __syscall_ipc.c fsync.c setdomainname.c uname.c modify_ldt.c \
	adjtimex.c mprotect.c sigprocmask.c init_module.c \
	delete_module.c get_kernel_syms.c quotactl.c getpgid.c \
	fchdir.c bdflush.c sysfs.c personality.c setfsuid.c \
	setfsgid.c flock.c msync.c \
	readv.c writev.c getsid.c fdatasync.c sysctl.c mlock.c \
	munlock.c mlockall.c munlockall.c sched_setparam.c \
	sched_getparam.c sched_setscheduler.c \
	sched_getscheduler.c sched_yield.c sched_get_priority_max.c \
	sched_get_priority_min.c sched_rr_get_interval.c \
	nanosleep.c mremap.c setresuid.c \
	getresuid.c query_module.c setresgid.c \
	getresgid.c prctl.c __syscall_rt_sigaction.c \
	__rt_sigtimedwait.c \
	chown.c capget.c capset.c sigaltstack.c \
	sendfile.c stat64.c lstat64.c \
	fstat64.c pivot_root.c madvise.c __syscall_fcntl.c __syscall_fcntl64.c

ifneq ($(strip $(EXCLUDE_BRK)),y)
CSRC+=sbrk.c
endif
ifeq ($(strip $(UCLIBC_PROFILING)),y)
CSRC+=gmon.c
endif
ifeq ($(strip $(UCLIBC_PROPOLICE)),y)
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
