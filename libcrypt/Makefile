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
LIBCRYPT=libcrypt.a
SHARED_NAME=libcrypt.so.$(MAJOR_VERSION)
TARGET_CC= ../extra/gcc-uClibc/$(TARGET_ARCH)-uclibc-gcc

CSRC = crypt.c des.c md5.c
COBJS=$(patsubst %.c,%.o, $(CSRC))


OBJS=$(COBJS)

all: $(OBJS) $(LIBCRYPT) shared

$(LIBCRYPT): ar-target

ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBCRYPT) $(OBJS)

$(COBJS): %.o : %.c
	$(TARGET_CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(OBJ): Makefile

shared: $(LIBCRYPT)
	$(TARGET_CC) $(LDFLAGS) -shared -o $(SHARED_NAME) \
	    -Wl,-soname,$(SHARED_NAME) $(COBJS) $(TOPDIR)$(SHARED_FULLNAME)

clean: 
	rm -f *.[oa] *~ core $(SHARED_NAME)

.PHONY: dummy


