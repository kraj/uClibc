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

COBJ-LIBC-ST = $(patsubst %.c, %.o, $(wildcard *.c))

AOBJ-LIBP-ST = $(patsubst %.S, %.o, $(wildcard *.S))
AOBJ-LIBP-SH = $(patsubst %.S, %.os, $(wildcard *.S))

ar-target all: $(AOBJ-LIBP-ST) $(AOBJ-LIBP-SH)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment	\
		$(AOBJ-LIBP-ST) $(AOBJ-LIBP-SH)
else
	$(STRIPTOOL) -x -R .note -R .comment $(AOBJ-LIBP-ST) $(AOBJ-LIBP-SH)
endif
	$(AR) $(ARFLAGS) $(PTDIR)../$(LIBPTHREAD_STATIC_ARCHIVE) $(AOBJ-LIBP-ST)
	$(AR) $(ARFLAGS) $(PTDIR)../$(LIBPTHREAD_SHARED_ARCHIVE) $(AOBJ-LIBP-SH)

$(AOBJ-LIBP-ST): %.o : %.S
	$(CC) $(ASFLAGS) $(ASFLAGS-NPTL) -c $< -o $@

$(AOBJ-LIBP-SH): %.os : %.S
	$(CC) $(ASFLAGS) $(ASFLAGS-NPTL) -DSHARED -c $< -o $@

$(COBJ-LIBC-ST): %.o : %.c
	$(CC) $(CFLAGS-LIBC) $(CFLAGS-$<) -c $< -o $@

objs-pthread-libc: $(COBJ-LIBC-ST)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment $(COBJ-LIBC-ST)
else
	$(STRIPTOOL) -x -R .note -R .comment $(COBJ-LIBC-ST)
endif
	@for objfile in $(COBJ-LIBC-ST); do \
		echo ../libpthread/nptl/sysdeps/mips/$$objfile >> $(PTCOBJST); \
	done

clean:
	$(RM) *.o *.os *~ core
