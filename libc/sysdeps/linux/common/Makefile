# Makefile for uClibc
#
# Copyright (C) 2000 by Lineo, inc.
# Copyright (C) 2000,2001 Erik Andersen <andersen@uclibc.org>
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
#
# Derived in part from the Linux-8086 C library, the GNU C Library, and several
# other sundry sources.  Files within this library are copyright by their
# respective copyright holders.

TOPDIR=../../../../
include $(TOPDIR)Rules.mak

CSRC=	waitpid.c kernel_version.c statfix.c getdnnm.c gethstnm.c \
	mkfifo.c setegid.c wait.c errno.c getpagesize.c seteuid.c \
	wait3.c setpgrp.c getdtablesize.c create_module.c ptrace.c \
	cmsg_nxthdr.c open64.c statfix64.c statfs64.c sbrk.c
ifneq ($(strip $(EXCLUDE_BRK)),true)
CSRC+=sbrk.c
endif
COBJS=$(patsubst %.c,%.o, $(CSRC))

MSRC=syscalls.c
MOBJ=$(shell ./list_syscalls.sh)

OBJ=$(COBJS) $(MOBJ)

all: $(STR_SYSCALLS) $(OBJ) $(LIBC)

$(LIBC): ar-target

ar-target: $(OBJ)
	$(AR) $(ARFLAGS) $(LIBC) $(OBJ)

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

clean:
	rm -f *.[oa] *~ core

