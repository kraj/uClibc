# Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk> Makefile
# for uClibc
#
# Copyright (C) 2000 by Lineo, inc.
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

TOPDIR=../../../
include $(TOPDIR)Rules.mak
LIBC=$(TOPDIR)libc.a

CSRC=	_xmknod.c waitpid.c getdents.c kernel_version.c rewinddir.c statfix.c \
	getdnnm.c libc_init.c seekdir.c telldir.c tell.c gethstnm.c mkfifo.c \
	setegid.c wait.c errno.c closedir.c getpagesize.c opendir.c seteuid.c \
	wait3.c dirfd.c isatty.c readdir.c setpgrp.c
COBJS=$(patsubst %.c,%.o, $(CSRC))

NISRC= _fxstat.c _lxstat.c _xstat.c
NIOBJS=$(patsubst %.c,%.o, $(NISRC))

MSRC=syscalls.c
MOBJ=$(shell ./list_syscalls.sh)

OBJ=$(COBJS) $(NIOBJS) $(MOBJ)

UNIFIED_SYSCALL_HEADER = /dev/null
STR_SYSCALLS =
ifeq ($(UNIFIED_SYSCALL),true)
	ifeq ($(TARGET_ARCH), i386)
		UNIFIED_SYSCALL_HEADER = unified_syscall_i386.h
		STR_SYSCALLS = str_syscalls
	endif
endif

all: $(STR_SYSCALLS) unified_syscall.h $(OBJ) $(LIBC)

$(LIBC): ar-target

ar-target: $(OBJ)
	$(AR) $(ARFLAGS) $(LIBC) $(OBJ)

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(NIOBJS):
	$(CC) $(CFLAGS) $< -c $*.c -o $*.o -fno-inline
	$(STRIPTOOL) -x -R .note -R .comment $*.o

str_syscalls:
	./str_syscalls.sh > str_syscalls.c
	gcc str_syscalls.c -o str_syscalls
	./str_syscalls > str_syscalls.h

unified_syscall.h:
	cat $(UNIFIED_SYSCALL_HEADER) > unified_syscall.h

clean:
	rm -f *.[oa] *~ core unified_syscall.h str_syscalls.[ch] str_syscalls

