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

ASRC-LIBP-ST = pt-vfork.S
ASRC-LIBP-SH = $(ASRC-LIBP-ST)

CSRC-LIBP-ST = pthread_once.c
CSRC-LIBP-SH = $(CSRC-LIBP-ST)

CSRC-LIBC-ST = fork.c
CSRC-LIBC-SH = $(CSRC-LIBC-ST)

AOBJ-LIBP-ST = $(patsubst %.S, %.o, $(ASRC-LIBP-ST))
AOBJ-LIBP-SH = $(patsubst %.S, %.os, $(ASRC-LIBP-SH))
COBJ-LIBP-ST = $(patsubst %.c, %.o, $(CSRC-LIBP-ST))
COBJ-LIBP-SH = $(patsubst %.c, %.os, $(CSRC-LIBP-ST))
COBJ-LIBC-ST = $(patsubst %.c, %.o, $(CSRC-LIBC-ST))
COBJ-LIBC-SH = $(patsubst %.c, %.os, $(CSRC-LIBC-SH))

ifeq ($(UCLIBC_HAS_STDIO_FUTEXES),y)
CFLAGS-fork.c = -D__USE_STDIO_FUTEXES__
endif

ar-target all: $(AOBJ-LIBP-ST) $(AOBJ-LIBP-SH) $(COBJ-LIBP-ST) $(COBJ-LIBP-SH)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment	\
		$(AOBJ-LIBP-ST) $(AOBJ-LIBP-SH) $(COBJ-LIBP-ST) $(COBJ-LIBP-SH)
else
	$(STRIPTOOL) -x -R .note -R .comment			\
		$(AOBJ-LIBP-ST) $(AOBJ-LIBP-SH) $(COBJ-LIBP-ST) $(COBJ-LIBP-SH)
endif
	$(AR) $(ARFLAGS) $(PTDIR)../$(LIBPTHREAD_STATIC_ARCHIVE)	\
		$(AOBJ-LIBP-ST) $(COBJ-LIBP-ST)
	$(AR) $(ARFLAGS) $(PTDIR)../$(LIBPTHREAD_SHARED_ARCHIVE)	\
		$(AOBJ-LIBP-SH) $(COBJ-LIBP-SH)

$(AOBJ-LIBP-ST): %.o : %.S
	$(CC) $(ASFLAGS-LIBP) -c $< -o $@

$(AOBJ-LIBP-SH): %.os : %.S
	$(CC) $(ASFLAGS-LIBP) -DSHARED -c $< -o $@

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
		echo ../libpthread/nptl/sysdeps/unix/sysv/linux/mips/$$objfile >> $(PTCOBJST); \
	done
	@for objfile in $(COBJ-LIBC-SH); do \
		echo ../libpthread/nptl/sysdeps/unix/sysv/linux/mips/$$objfile >> $(PTCOBJSH); \
	done

clean:
	$(RM) *.o *.os *~ core
