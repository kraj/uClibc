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


TOPDIR=../../
include $(TOPDIR)Rules.mak

TARGET_CFLAGS += #-DDL_DEBUG #-funroll-loops



LDSO_FULLNAME=ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).so
TARGET_CFLAGS+=-fPIC -D__PIC__ -DUCLIBC_PREFIX=\"$(PREFIX)\" \
	-DUCLIBC_DEVEL_PREFIX=\"$(DEVEL_PREFIX)\" \
	-DUCLIBC_BUILD_DIR=\"$(shell cd $(TOPDIR); pwd)\"
CSRC= boot1.c hash.c readelflib1.c $(TARGET_ARCH)/elfinterp.c
COBJS=$(patsubst %.c,%.o, $(CSRC))
ASRC=$(shell ls $(TARGET_ARCH)/*.S)
AOBJS=$(patsubst %.S,%.o, $(ASRC))
OBJS=$(AOBJS) $(COBJS)


all: lib

lib:: ld.so.h $(OBJS) $(DLINK_OBJS)
	$(TARGET_CC) -e _dl_boot $(TARGET_LDFLAGS) -nostdlib -shared -o $(LDSO_FULLNAME) \
	    -Wl,-soname,$(UCLIBC_LDSO) $(OBJS)
	install -d $(TOPDIR)lib 
	install -m 755 $(LDSO_FULLNAME) $(TOPDIR)lib
	(cd $(TOPDIR)lib;ln -sf $(LDSO_FULLNAME) $(UCLIBC_LDSO))

ld.so.h: Makefile
	echo "#define _dl_static_progname \""$(UCLIBC_LDSO)"\"" > ld.so.h

$(COBJS): %.o : %.c
	$(TARGET_CC) $(TARGET_CFLAGS) -I. -I./$(TARGET_ARCH) -I../libdl -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

clean::
	$(RM) -f $(UCLIBC_LDSO)* $(OBJS) $(LDSO_FULLNAME)* core *.o *.a *.s *.i tmp_make foo *~

