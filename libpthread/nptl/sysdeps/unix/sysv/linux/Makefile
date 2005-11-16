# Makefile for uClibc NPTL pthread library (Linux files)
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

TOPDIR=../../../../../../
include $(TOPDIR)Rules.mak
include ../../../../Rules.mak

AR_LIBP_CSRC = pt-raise.c pthread_attr_getaffinity.c		\
	       pthread_attr_setaffinity.c pthread_getaffinity.c	\
	       pthread_getcpuclockid.c pthread_kill.c		\
	       pthread_mutex_cond_lock.c pthread_setaffinity.c	\
	       pthread_yield.c sem_post.c sem_timedwait.c	\
	       sem_trywait.c sem_wait.c pt-fork.c
SO_LIBP_CSRC = $(AR_LIBP_CSRC) 
AR_LIBC_CSRC = libc_pthread_init.c libc_multiple_threads.c	\
	       register-atfork.c unregister-atfork.c
SO_LIBC_CSRC = $(AR_LIBC_CSRC)

ifeq ($(strip $(TARGET_ARCH)),alpha)
AR_LIBC_CSRC += libc-lowlevellock.c
AR_LIBP_CSRC += lowlevellock.c
endif
ifeq ($(strip $(TARGET_ARCH)),mips)
AR_LIBC_CSRC += libc-lowlevellock.c
AR_LIBP_CSRC += lowlevellock.c
endif
ifeq ($(strip $(TARGET_ARCH)),powerpc)
AR_LIBC_CSRC += libc-lowlevellock.c
AR_LIBP_CSRC += lowlevellock.c
endif
ifeq ($(strip $(TARGET_ARCH)),sparc)
AR_LIBC_CSRC += libc-lowlevellock.c
AR_LIBP_CSRC += lowlevellock.c
endif

#
# Needed to get pthread types.
#
CFLAGS-pt-fork.c = -D_GNU_SOURCE
CFLAGS-pthread_getcpuclockid.c = -I$(TOPDIR)/librt

AR_LIBP_COBJS = $(patsubst %.c, %.o, $(AR_LIBP_CSRC))
SO_LIBP_COBJS = $(patsubst %.c, %.os, $(SO_LIBP_CSRC))
AR_LIBC_COBJS = $(patsubst %.c, %.o, $(AR_LIBC_CSRC))
SO_LIBC_COBJS = $(patsubst %.c, %.os, $(SO_LIBC_CSRC))

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

obj.pthread.ar: $(AR_LIBC_COBJS)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment $(AR_LIBC_COBJS)
else
	$(STRIPTOOL) -x -R .note -R .comment $(AR_LIBC_COBJS)
endif
	@for objfile in $(AR_LIBC_COBJS); do \
		echo ../libpthread/nptl/sysdeps/unix/sysv/linux/$$objfile >> \
			$(TOPDIR)libc/$@; \
	done

obj.pthread.so: $(SO_LIBC_COBJS)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment $(SO_LIBC_COBJS)
else
	$(STRIPTOOL) -x -R .note -R .comment $(SO_LIBC_COBJS)
endif
	@for objfile in $(SO_LIBC_COBJS); do \
		echo ../libpthread/nptl/sysdeps/unix/sysv/linux/$$objfile >> \
			$(TOPDIR)libc/$@; \
	done

$(AR_LIBP_COBJS): %.o : %.c
	$(CC) $(CFLAGS-$<) $(CFLAGS-LIBP) -c $< -o $@

$(SO_LIBP_COBJS): %.os : %.c
	$(CC) $(CFLAGS-$<) $(CFLAGS-LIBP) -DSHARED -c $< -o $@

$(AR_LIBC_COBJS): %.o : %.c
	$(CC) $(CFLAGS-$<) $(CFLAGS-LIBC) -c $< -o $@

$(SO_LIBC_COBJS): %.os : %.c
	$(CC) $(CFLAGS-$<) $(CFLAGS-LIBC) -DSHARED -c $< -o $@

clean:
	$(RM) *.o *.os *~ core

.PHONY: $(AR_LIB_NAME) $(AR_SO_LIB_NAME)
