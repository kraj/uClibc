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
       unwind-forcedunwind.c

#
# Needed to get pthread types.
#
CFLAGS-pthread_barrier_wait.c = -D_GNU_SOURCE
CFLAGS-pthread_spin_destroy.c = -D_GNU_SOURCE
CFLAGS-pthread_spin_init.c = -D_GNU_SOURCE
CFLAGS-pthread_spin_unlock.c = -D_GNU_SOURCE

AR_LIBP_COBJS = $(patsubst %.c, %.o, $(CSRC))
SO_LIBP_COBJS = $(patsubst %.c, %.os, $(CSRC))

all: $(AR_LIB_NAME) $(AR_SO_LIB_NAME)

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
	$(CC) $(CFLAGS-$<) $(CFLAGS-LIBP) -c $< -o $@

$(SO_LIBP_COBJS): %.os : %.c
	$(CC) $(CFLAGS-$<) $(CFLAGS-LIBP) -DSHARED -c $< -o $@

clean:
	$(RM) *.o *.os *~ core

.PHONY: $(AR_LIB_NAME) $(AR_SO_LIB_NAME)
