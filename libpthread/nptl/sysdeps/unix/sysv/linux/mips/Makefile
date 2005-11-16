# Makefile for uClibc NPTL pthread library (Linux/MIPS)
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

TOPDIR=../../../../../../../
include $(TOPDIR)Rules.mak
include ../../../../../Rules.mak

#AR_LIBP_ASRC = pt-vfork.S clone.S
AR_LIBP_ASRC = pt-vfork.S
SO_LIBP_ASRC = $(AR_LIBP_ASRC)
AR_LIBP_CSRC = pthread_once.c
SO_LIBP_CSRC = $(AR_LIBP_CSRC)
AR_LIBC_CSRC = fork-nptl.c
SO_LIBC_CSRC = $(AR_LIBC_CSRC)

AR_LIBP_AOBJS = $(patsubst %.S, %.o, $(AR_LIBP_ASRC))
SO_LIBP_AOBJS = $(patsubst %.S, %.os, $(SO_LIBP_ASRC))
AR_LIBP_COBJS = $(patsubst %.c, %.o, $(AR_LIBP_CSRC))
SO_LIBP_COBJS = $(patsubst %.c, %.os, $(SO_LIBP_CSRC))
AR_LIBC_COBJS = $(patsubst %.c, %.o, $(AR_LIBC_CSRC))
SO_LIBC_COBJS = $(patsubst %.c, %.os, $(SO_LIBC_CSRC))

ifeq ($(UCLIBC_HAS_STDIO_FUTEXES),y)
CFLAGS-fork-nptl.c = -D__USE_STDIO_FUTEXES__
endif

all: $(AR_LIB_NAME) $(AR_SO_LIB_NAME)

$(AR_LIB_NAME): $(AR_LIBP_AOBJS) $(AR_LIBP_COBJS)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment	\
		$(AR_LIBP_AOBJS) $(AR_LIBP_COBJS)
else
	$(STRIPTOOL) -x -R .note -R .comment			\
		$(AR_LIBP_AOBJS) $(AR_LIBP_COBJS)
endif
	$(AR) $(ARFLAGS) $(AR_LIB_NAME) $(AR_LIBP_AOBJS) $(AR_LIBP_COBJS)

$(AR_SO_LIB_NAME): $(SO_LIBP_AOBJS) $(SO_LIBP_COBJS)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment	\
		$(SO_LIBP_AOBJS) $(SO_LIBP_COBJS)
else
	$(STRIPTOOL) -x -R .note -R .comment			\
		$(SO_LIBP_AOBJS) $(SO_LIBP_COBJS)
endif
	$(AR) $(ARFLAGS) $(AR_SO_LIB_NAME) $(SO_LIBP_AOBJS) $(SO_LIBP_COBJS)

obj.pthread.ar: $(AR_LIBC_COBJS)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment $(AR_LIBC_COBJS)
else
	$(STRIPTOOL) -x -R .note -R .comment $(AR_LIBC_COBJS)
endif
	@for objfile in $(AR_LIBC_COBJS); do \
		echo ../libpthread/nptl/sysdeps/unix/sysv/linux/mips/$$objfile \
			>> $(TOPDIR)libc/$@; \
	done

obj.pthread.so: $(SO_LIBC_COBJS)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment $(SO_LIBC_COBJS)
else
	$(STRIPTOOL) -x -R .note -R .comment $(SO_LIBC_COBJS)
endif
	@for objfile in $(SO_LIBC_COBJS); do \
		echo ../libpthread/nptl/sysdeps/unix/sysv/linux/mips/$$objfile \
			>> $(TOPDIR)libc/$@; \
	done

$(AR_LIBP_AOBJS): %.o : %.S
	$(CC) $(ASFLAGS-LIBP) -c $< -o $@

$(SO_LIBP_AOBJS): %.os : %.S
	$(CC) $(ASFLAGS-LIBP) -DSHARED -c $< -o $@

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
