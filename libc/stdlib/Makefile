# Makefile for uClibc
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

TOPDIR=../
include $(TOPDIR)Rules.mak
LIBC=$(TOPDIR)libc.a

DIRS = $(MALLOC)

MSRC=strto_l.c
MOBJ=strtol.o strtoul.o strto_l.o

MSRC1=strto_ll.c
MOBJ1=strtoll.o strtoull.o strto_ll.o

MSRC2=atexit.c
MOBJ2=on_exit.o atexit.o __do_exit.o exit.o


CSRC =	abort.c getenv.c mktemp.c qsort.c realpath.c abs.c bsearch.c \
	mkstemp.c putenv.c rand.c setenv.c system.c
ifeq ($(HAS_FLOATS),true)
	CSRC += strtod.c
endif
COBJS=$(patsubst %.c,%.o, $(CSRC))


OBJS=$(MOBJ) $(MOBJ1) $(MOBJ2) $(COBJS)

all: $(OBJS) $(LIBC)

$(LIBC): ar-target subdirs

ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBC) $(OBJS)

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJ1): $(MSRC1)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJ2): $(MSRC2)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(COBJS):
	$(CC) $(CFLAGS) $< -c $*.c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(OBJ): Makefile

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(DIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

clean: subdirs_clean
	rm -f *.[oa] *~ core

.PHONY: dummy


