# Makefile for uClibc's NPTL pthread library (Portions stolen from glibc.)
#
# Copyright (C) 2005 Steven J. Hill <sjhill@realitydiluted.com>
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
# Makefile for uClibc

TOPDIR=../../
include $(TOPDIR)Rules.mak
include Rules.mak

routines = alloca_cutoff forward libc-lowlevellock libc-cancellation
shared-only-routines = forward

libpthread-routines = init vars events version \
		      pthread_create pthread_exit pthread_detach \
		      pthread_join pthread_tryjoin pthread_timedjoin \
		      pthread_self pthread_equal pthread_yield \
		      pthread_getconcurrency pthread_setconcurrency \
		      pthread_getschedparam pthread_setschedparam \
		      pthread_setschedprio \
		      pthread_attr_init pthread_attr_destroy \
		      pthread_attr_getdetachstate pthread_attr_setdetachstate \
		      pthread_attr_getguardsize pthread_attr_setguardsize \
		      pthread_attr_getschedparam pthread_attr_setschedparam \
		      pthread_attr_getschedpolicy pthread_attr_setschedpolicy \
		      pthread_attr_getinheritsched \
		      pthread_attr_setinheritsched \
		      pthread_attr_getscope pthread_attr_setscope \
		      pthread_attr_getstackaddr pthread_attr_setstackaddr \
		      pthread_attr_getstacksize pthread_attr_setstacksize \
		      pthread_attr_getstack pthread_attr_setstack \
		      pthread_getattr_np \
		      pthread_mutex_init pthread_mutex_destroy \
		      pthread_mutex_lock pthread_mutex_trylock \
		      pthread_mutex_timedlock pthread_mutex_unlock \
		      pthread_mutexattr_init pthread_mutexattr_destroy \
		      pthread_mutexattr_getpshared \
		      pthread_mutexattr_setpshared \
		      pthread_mutexattr_gettype pthread_mutexattr_settype \
		      pthread_rwlock_init pthread_rwlock_destroy \
		      pthread_rwlock_rdlock pthread_rwlock_timedrdlock \
		      pthread_rwlock_wrlock pthread_rwlock_timedwrlock \
		      pthread_rwlock_tryrdlock pthread_rwlock_trywrlock \
		      pthread_rwlock_unlock \
		      pthread_rwlockattr_init pthread_rwlockattr_destroy \
		      pthread_rwlockattr_getpshared \
		      pthread_rwlockattr_setpshared \
		      pthread_rwlockattr_getkind_np \
		      pthread_rwlockattr_setkind_np \
		      pthread_cond_init pthread_cond_destroy \
		      pthread_cond_wait pthread_cond_timedwait \
		      pthread_cond_signal pthread_cond_broadcast \
		      old_pthread_cond_init old_pthread_cond_destroy \
		      old_pthread_cond_wait old_pthread_cond_timedwait \
		      old_pthread_cond_signal old_pthread_cond_broadcast \
		      pthread_condattr_init pthread_condattr_destroy \
		      pthread_condattr_getpshared pthread_condattr_setpshared \
		      pthread_condattr_getclock pthread_condattr_setclock \
		      pthread_spin_init pthread_spin_destroy \
		      pthread_spin_lock pthread_spin_trylock \
		      pthread_spin_unlock \
		      pthread_barrier_init pthread_barrier_destroy \
		      pthread_barrier_wait \
		      pthread_barrierattr_init pthread_barrierattr_destroy \
		      pthread_barrierattr_getpshared \
		      pthread_barrierattr_setpshared \
		      pthread_key_create pthread_key_delete \
		      pthread_getspecific pthread_setspecific \
		      pthread_sigmask pthread_kill \
		      pthread_cancel pthread_testcancel \
		      pthread_setcancelstate pthread_setcanceltype \
		      pthread_once \
		      old_pthread_atfork pthread_atfork \
		      pthread_getcpuclockid \
		      pthread_clock_gettime pthread_clock_settime \
		      sem_init sem_destroy \
		      sem_open sem_close sem_unlink \
		      sem_getvalue \
		      sem_wait sem_trywait sem_timedwait sem_post \
		      cleanup cleanup_defer cleanup_compat \
		      cleanup_defer_compat unwind \
		      pt-longjmp pt-cleanup\
		      cancellation \
		      lowlevellock \
		      pt-vfork \
		      ptw-write ptw-read ptw-close ptw-fcntl ptw-accept \
		      ptw-connect ptw-recv ptw-recvfrom ptw-recvmsg ptw-send \
		      ptw-sendmsg ptw-sendto ptw-fsync ptw-lseek ptw-llseek \
		      ptw-msync ptw-nanosleep ptw-open ptw-open64 ptw-pause \
		      ptw-pread ptw-pread64 ptw-pwrite ptw-pwrite64 \
		      ptw-tcdrain ptw-wait ptw-waitpid ptw-msgrcv ptw-msgsnd \
		      ptw-sigwait \
		      pt-raise pt-system \
		      flockfile ftrylockfile funlockfile \
		      sigaction \
		      herrno res pt-allocrtsig \
		      pthread_kill_other_threads \
		      pthread_getaffinity pthread_setaffinity \
		      pthread_attr_getaffinity pthread_attr_setaffinity \
		      cleanup_routine unwind-forcedunwind
#		      pthread_setuid pthread_seteuid pthread_setreuid \
#		      pthread_setresuid \
#		      pthread_setgid pthread_setegid pthread_setregid \
#		      pthread_setresgid

libpthread-shared-only-routines = version pt-allocrtsig unwind-forcedunwind
libpthread-static-only-routines = pthread_atfork

CFLAGS-pthread_atfork.c = -DNOT_IN_libc

# Since cancellation handling is in large parts handled using exceptions
# we have to compile some files with exception handling enabled, some
# even with asynchronous unwind tables.

# init.c contains sigcancel_handler().
CFLAGS-init.c = -fexceptions -fasynchronous-unwind-tables
# The unwind code itself,
CFLAGS-unwind.c = -fexceptions
CFLAGS-unwind-forcedunwind.c = -fexceptions -fasynchronous-unwind-tables

# The following three functions must be async-cancel safe.
CFLAGS-pthread_cancel.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-pthread_setcancelstate.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-pthread_setcanceltype.c = -fexceptions -fasynchronous-unwind-tables

# These are internal functions which similar functionality as setcancelstate
# and setcanceltype.
CFLAGS-cancellation.c = -fasynchronous-unwind-tables
CFLAGS-libc-cancellation.c = -fasynchronous-unwind-tables

# Calling pthread_exit() must cause the registered cancel handlers to
# be executed.  Therefore exceptions have to be thrown through this
# function.
CFLAGS-pthread_exit.c = -fexceptions

# Among others, __pthread_unwind is forwarded.  This function must handle
# exceptions.
CFLAGS-forward.c = -fexceptions

# The following are cancellation points.  Some of the functions can
# block and therefore temporarily enable asynchronous cancellation.
# Those must be compiled asynchronous unwind tables.
CFLAGS-pthread_testcancel.c = -fexceptions
CFLAGS-pthread_join.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-pthread_timedjoin.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-pthread_once.c = $(uses-callbacks) -fexceptions \
			-fasynchronous-unwind-tables
CFLAGS-pthread_cond_wait.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-pthread_cond_timedwait.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-sem_wait.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-sem_timedwait.c = -fexceptions -fasynchronous-unwind-tables

# These are the function wrappers we have to duplicate here.
CFLAGS-fcntl.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-lockf.c = -fexceptions
CFLAGS-pread.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-pread64.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-pwrite.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-pwrite64.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-wait.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-waitpid.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-sigwait.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-msgrcv.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-msgsnd.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-tcdrain.c = -fexceptions -fasynchronous-unwind-tables

CFLAGS-pt-system.c = -fexceptions

#
# The rest of this file is uClibc specific.
#
CFLAGS-pthread_barrier_init.c = -D_GNU_SOURCE
CFLAGS-pthread_barrier_destroy.c = -D_GNU_SOURCE
CFLAGS-pthread_barrierattr_init.c = -D_GNU_SOURCE
CFLAGS-pthread_barrierattr_destroy.c = -D_GNU_SOURCE
CFLAGS-pthread_barrierattr_getpshared.c = -D_GNU_SOURCE
CFLAGS-pthread_barrierattr_setpshared.c = -D_GNU_SOURCE
CFLAGS-sem_open.c = -D_GNU_SOURCE

libpthread-os-routines = pthread_yield
libpthread-misc-routines = pthread_rwlock_rdlock pthread_rwlock_timedrdlock \
			   pthread_rwlock_wrlock pthread_rwlock_timedwrlock \
			   pthread_rwlock_unlock pthread_cond_wait \
			   pthread_cond_timedwait pthread_cond_signal \
			   pthread_cond_broadcast pthread_spin_init \
			   pthread_spin_destroy pthread_spin_lock \
			   pthread_spin_trylock pthread_spin_unlock \
			   pthread_barrier_wait pthread_sigmask \
			   pthread_kill pthread_once \
			   pthread_getcpuclockid sem_wait \
			   sem_trywait sem_timedwait \
			   sem_post pt-longjmp \
			   lowlevellock pt-vfork \
			   ptw-close ptw-read ptw-write \
			   ptw-fcntl ptw-accept ptw-connect \
			   ptw-recv ptw-recvfrom ptw-recvmsg \
			   ptw-send ptw-sendmsg ptw-sendto \
			   ptw-fsync ptw-lseek ptw-llseek \
			   ptw-msync ptw-nanosleep ptw-open \
			   ptw-open64 ptw-pause ptw-pread \
			   ptw-pread64 ptw-pwrite ptw-pwrite64 \
			   ptw-tcdrain ptw-wait ptw-waitpid \
			   ptw-msgrcv ptw-msgsnd ptw-sigwait \
			   pt-raise flockfile ftrylockfile \
			   funlockfile sigaction \
			   pthread_kill_other_threads \
			   pthread_getaffinity \
			   pthread_setaffinity \
			   pthread_attr_getaffinity \
			   pthread_attr_setaffinity \
			   unwind-forcedunwind

DIRS := sysdeps/pthread sysdeps/$(TARGET_ARCH) sysdeps/unix/sysv/linux	\
	sysdeps/unix/sysv/linux/$(TARGET_ARCH)

CSRC-LIBP-ST = $(patsubst %, %.c, $(filter-out $(libpthread-os-routines) \
	       $(libpthread-misc-routines)				 \
	       $(libpthread-shared-only-routines),			 \
	       $(libpthread-routines)))

CSRC-LIBP-SH = $(patsubst %, %.c, $(filter-out $(libpthread-os-routines) \
	       $(libpthread-misc-routines)				 \
	       $(libpthread-static-only-routines) pt-allocrtsig,	 \
	       $(libpthread-routines)))

CSRC-LIBC-ST = $(patsubst %, %.c, $(filter-out $(shared-only-routines)	 \
	       libc-lowlevellock, $(routines)))

CSRC-LIBC-SH = $(patsubst %, %.c, $(filter-out libc-lowlevellock, $(routines)))

COBJ-LIBP-ST = $(patsubst %.c, %.o, $(CSRC-LIBP-ST))
COBJ-LIBP-SH = $(patsubst %.c, %.os, $(CSRC-LIBP-SH))
COBJ-LIBC-ST = $(patsubst %.c, %.o, $(CSRC-LIBC-ST))
COBJ-LIBC-SH = $(patsubst %.c, %.os, $(CSRC-LIBC-SH))

ar-target all: version.h banner.h $(COBJ-LIBP-ST) $(COBJ-LIBP-SH) subdirs
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment	\
		$(COBJ-LIBP-ST) $(COBJ-LIBP-SH)
else
	$(STRIPTOOL) -x -R .note -R .comment			\
		$(COBJ-LIBP-ST) $(COBJ-LIBP-SH)
endif
	$(AR) $(ARFLAGS) $(PTDIR)../$(LIBPTHREAD_STATIC_ARCHIVE) $(COBJ-LIBP-ST)
	$(AR) $(ARFLAGS) $(PTDIR)../$(LIBPTHREAD_SHARED_ARCHIVE) $(COBJ-LIBP-SH)

version.h:
	@echo "#define VERSION \""$(VERSION)"\"" > $@

banner.h: Banner
	sed 's/\(.*\)/"\1\\n"/' $< > $@

$(COBJ-LIBP-ST): %.o : %.c
	$(CC) $(CFLAGS-$<) $(CFLAGS-LIBP) -c $< -o $@

$(COBJ-LIBP-SH): %.os : %.c
	$(CC) $(CFLAGS-$<) $(CFLAGS-LIBP) -DSHARED -c $< -o $@

$(COBJ-LIBC-ST): %.o : %.c
	$(CC) $(CFLAGS-$<) $(CFLAGS-LIBC) -c $< -o $@

$(COBJ-LIBC-SH): %.os : %.c
	$(CC) $(CFLAGS-$<) $(CFLAGS-LIBC) -DSHARED -c $< -o $@

objs-pthread-libc: $(COBJ-LIBC-ST) $(COBJ-LIBC-SH)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment	\
		$(COBJ-LIBC-ST) $(COBJ-LIBC-SH)
else
	$(STRIPTOOL) -x -R .note -R .comment			\
		$(COBJ-LIBC-ST) $(COBJ-LIBC-SH)
endif
	@for objfile in $(COBJ-LIBC-ST); do \
		echo ../libpthread/nptl/$$objfile >> $(PTCOBJST); \
	done
	@for objfile in $(COBJ-LIBC-SH); do \
		echo ../libpthread/nptl/$$objfile >> $(PTCOBJSH); \
	done
	$(MAKE) -C sysdeps/$(TARGET_ARCH) $@
	$(MAKE) -C sysdeps/unix/sysv/linux $@
	$(MAKE) -C sysdeps/unix/sysv/linux/$(TARGET_ARCH) $@

clean: subdirs_clean
	$(RM) *.o *.os *~ core banner.h version.h

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(DIRS))

$(patsubst %, _dir_%, $(DIRS)):
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(DIRS)):
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

.PHONY: dummy
