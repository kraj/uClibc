# Makefile for uClibc NPTL pthread library (MIPS)
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

CFLAGS-dl-support.c = -D_GNU_SOURCE

AR_LIBP_AOBJS = $(patsubst %.S, %.o, $(wildcard *.S))
SO_LIBP_AOBJS = $(patsubst %.S, %.os, $(wildcard *.S))
AR_LIBC_COBJS = $(patsubst %.c, %.o, $(wildcard *.c))

all: $(AR_LIB_NAME) $(AR_SO_LIB_NAME)

$(AR_LIB_NAME): $(AR_LIBP_AOBJS)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment $(AR_LIBP_AOBJS)
else
	$(STRIPTOOL) -x -R .note -R .comment $(AR_LIBP_AOBJS) $(AR_LIBP_AOBJS)
endif
	$(AR) $(ARFLAGS) $(AR_LIB_NAME) $(AR_LIBP_AOBJS)

$(AR_SO_LIB_NAME): $(SO_LIBP_AOBJS)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment $(SO_LIBP_AOBJS)
else
	$(STRIPTOOL) -x -R .note -R .comment $(AR_LIBP_AOBJS) $(SO_LIBP_AOBJS)
endif
	$(AR) $(ARFLAGS) $(AR_SO_LIB_NAME) $(SO_LIBP_AOBJS)

obj.pthread.ar: $(AR_LIBC_COBJS)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment $(AR_LIBC_COBJS)
else
	$(STRIPTOOL) -x -R .note -R .comment $(AR_LIBC_COBJS)
endif
	@for objfile in $(AR_LIBC_COBJS); do \
		echo ../libpthread/nptl/sysdeps/mips/$$objfile >> \
			$(TOPDIR)libc/$@; \
	done

$(AR_LIBP_AOBJS): %.o : %.S
	$(CC) $(ASFLAGS) $(ASFLAGS-NPTL) -c $< -o $@

$(SO_LIBP_AOBJS): %.os : %.S
	$(CC) $(ASFLAGS) $(ASFLAGS-NPTL) -DSHARED -c $< -o $@

$(AR_LIBC_COBJS): %.o : %.c
	$(CC) $(CFLAGS-LIBC) $(CFLAGS-$<) -c $< -o $@

clean:
	$(RM) *.o *.os *~ core

.PHONY: $(AR_LIB_NAME) $(AR_SO_LIB_NAME)
