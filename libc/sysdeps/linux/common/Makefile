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

SAFECFLAGS=$(WARNINGS) $(OPTIMIZATION) -fno-builtin
ifeq ($(strip $(DOPIC)),true)
SAFECFLAGS+=-fPIC
endif

CSRC=	waitpid.c statfix.c getdnnm.c gethstnm.c getcwd.c \
	mkfifo.c setegid.c wait.c getpagesize.c seteuid.c \
	wait3.c setpgrp.c getdtablesize.c create_module.c ptrace.c \
	cmsg_nxthdr.c statfix64.c longjmp.c open64.c ftruncate64.c \
	truncate64.c getrlimit64.c setrlimit64.c creat64.c mmap64.c
ifneq ($(strip $(EXCLUDE_BRK)),true)
CSRC+=sbrk.c
endif
COBJS=$(patsubst %.c,%.o, $(CSRC))

MSRC=syscalls.c
MOBJ=$(shell ./list_syscalls.sh)

OBJ=$(COBJS) $(MOBJ)

all: $(STR_SYSCALLS) $(OBJ) $(LIBC)

$(LIBC): ar-target

ar-target: $(OBJ) $(TOPDIR)lib/crti.o $(TOPDIR)lib/crtn.o
	$(AR) $(ARFLAGS) $(LIBC) $(OBJ)

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

initfini.s: initfini.c
	$(CC) $(SAFECFLAGS) -c initfini.c -S -o initfini.s

crti.S crtn.S: initfini.s
	$(TOPDIR)/extra/scripts/initfini.pl

crti.o: crti.S
	$(CC) $(SAFECFLAGS) -c crti.S -o crti.o

$(TOPDIR)lib/crti.o: crti.o
	mkdir -p $(TOPDIR)lib/
	cp crti.o $(TOPDIR)lib/

crtn.o: crtn.S
	$(CC) $(SAFECFLAGS) -c crtn.S -o crtn.o

$(TOPDIR)lib/crtn.o: crtn.o
	mkdir -p $(TOPDIR)lib/
	cp crtn.o $(TOPDIR)lib/

clean:
	rm -f *.[oa] *~ core crti.* crtn.* initfini.s

