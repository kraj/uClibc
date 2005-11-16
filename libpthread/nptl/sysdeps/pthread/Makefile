# Makefile for uClibc NPTL pthread library (pthread)
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

TOPDIR=../../../../
include $(TOPDIR)Rules.mak
include ../../Rules.mak

#
# NOTE: glibc puts flockfile.c, ftrylockfile.c, funlockfile.c, and
#       pt-longjmp.c in libc and libpthread. For uClibc, they are
#       in libc only.
#
CSRC = pthread_barrier_wait.c pthread_cond_broadcast.c pthread_cond_signal.c \
       pthread_cond_timedwait.c pthread_cond_wait.c pthread_getcpuclockid.c  \
       pthread_once.c pthread_rwlock_rdlock.c pthread_rwlock_timedrdlock.c   \
       pthread_rwlock_timedwrlock.c pthread_rwlock_unlock.c		     \
       pthread_rwlock_wrlock.c pthread_sigmask.c pthread_spin_destroy.c      \
       pthread_spin_init.c pthread_spin_unlock.c sigaction.c		     \
       sigfillset.c sigprocmask.c unwind-forcedunwind.c

CFLAGS-pthread_barrier_wait.c = -D_GNU_SOURCE
CFLAGS-pthread_spin_destroy.c = -D_GNU_SOURCE
CFLAGS-pthread_spin_init.c = -D_GNU_SOURCE
CFLAGS-pthread_spin_unlock.c = -D_GNU_SOURCE
CFLAGS-sigaction.c = -I$(TOPDIR)libc/sysdeps/linux/$(TARGET_ARCH) \
		     -I$(TOPDIR)libc/signal
CFLAGS-sigfillset.c = -I$(TOPDIR)libc/sysdeps/linux/$(TARGET_ARCH) \
		      -I$(TOPDIR)libc/signal
CFLAGS-sigprocmask.c = -I$(TOPDIR)libc/sysdeps/linux/common

AR_LIBP_COBJS = $(patsubst %.c, %.o, $(CSRC))
SO_LIBP_COBJS = $(patsubst %.c, %.os, $(CSRC))

ifeq ($(strip $(UCLIBC_CTOR_DTOR)),y)
all: $(AR_LIB_NAME) $(AR_SO_LIB_NAME) crti.o crtn.o
else
all: $(AR_LIB_NAME) $(AR_SO_LIB_NAME)
endif

$(AR_LIB_NAME): $(AR_LIBP_COBJS)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment $(AR_LIBP_COBJS)
else
	$(STRIPTOOL) -x -R .note -R .comment $(AR_LIBP_COBJS)
endif
	$(AR) $(ARFLAGS) $(AR_LIB_NAME) $(AR_LIBP_COBJS)

$(AR_SO_LIB_NAME): $(SO_LIBP_COBJS)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment $(SO_LIBP_COBJS)
else
	$(STRIPTOOL) -x -R .note -R .comment $(SO_LIBP_COBJS)
endif
	$(AR) $(ARFLAGS) $(AR_SO_LIB_NAME) $(SO_LIBP_COBJS)

$(AR_LIBP_COBJS): %.o : %.c
	$(CC) $(CFLAGS-LIBP) $(CFLAGS-$<) -c $< -o $@

$(SO_LIBP_COBJS): %.os : %.c
	$(CC) $(CFLAGS-LIBP) $(CFLAGS-$<) -DSHARED -c $< -o $@

pt-initfini.s: pt-initfini.c
	$(CC) -S $(CFLAGS-LIBP) -g0 -fPIC -fno-inline-functions		\
		$(call check_gcc,-fno-unit-at-a-time,)			\
		-finhibit-size-directive				\
		$(patsubst -f%,-fno-%,$(call check_gcc,-fexceptions,))	\
		$< -o $@

crti.S: pt-initfini.s
	sed -n -e '1,/@HEADER_ENDS/p' \
	       -e '/@_.*_PROLOG_BEGINS/,/@_.*_PROLOG_ENDS/p' \
	       -e '/@TRAILER_BEGINS/,$$p' $< > $@

crtn.S: pt-initfini.s
	sed -n -e '1,/@HEADER_ENDS/p' \
	       -e '/@_.*_EPILOG_BEGINS/,/@_.*_EPILOG_ENDS/p' \
	       -e '/@TRAILER_BEGINS/,$$p' $< > $@

defs.h: pt-initfini.s defs.awk
	sed -n -e '/@TESTS_BEGIN/,/@TESTS_END/p' $< | \
		awk -f defs.awk > $@

crti.o: crti.S defs.h
	$(CC) -g0 $(ASFLAGS-LIBC) -c $< -o $@

crtn.o: crtn.S defs.h
	$(CC) -g0 $(ASFLAGS-LIBC) -c $< -o $@

clean:
	$(RM) *.o *.os *.s crt* *~ core defs.h

.PHONY: $(AR_LIB_NAME) $(AR_SO_LIB_NAME)
