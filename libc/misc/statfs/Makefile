# Makefile for uClibc
# Copyright (C) 2001,2002 Erik Andersen <andersen@uclibc.org>
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

CSRC = statvfs.c fstatvfs.c
ifeq ($(UCLIBC_HAS_LFS),y)
CSRC += fstatfs64.c statfs64.c statvfs64.c fstatvfs64.c
endif
OBJS := $(patsubst %.c,%.o, $(CSRC))

OBJ_LIST := ../../obj.misc.statfs

all: $(OBJ_LIST)

$(OBJ_LIST): $(OBJS)
	$(STRIPTOOL) -x -R .note -R .comment $^
	echo $(patsubst %, misc/statfs/%, $^) > $@

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o *~ core
