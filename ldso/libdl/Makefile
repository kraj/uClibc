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
include $(TOPDIR)/ldso/Rules.mak

LIBDL=libdl.a
LIBDL_SHARED=libdl.so
LIBDL_SHARED_FULLNAME=libdl-$(MAJOR_VERSION).$(MINOR_VERSION).so
TARGET_CC= $(TOPDIR)extra/gcc-uClibc/$(TARGET_ARCH)-uclibc-gcc

CSRC= dlib.c
OBJS=$(patsubst %.c,%.o, $(CSRC))

CFLAGS += -DUSE_CACHE #-fPIC -D__PIC__ #-funroll-loops

all: $(OBJS) $(LIBDL) shared

$(LIBDL): ar-target

ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBDL) $(OBJS)
	#install -d $(TOPDIR)lib
	#rm -f $(TOPDIR)lib/$(LIBDL)
	#install -m 644 $(LIBDL) $(TOPDIR)lib

$(OBJS): %.o : %.c
	$(TARGET_CC) -I../d-link -I../d-link/$(TARGET_ARCH) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(OBJ): Makefile

shared: all
	$(TARGET_CC) $(LDFLAGS) -shared -o $(LIBDL_SHARED_FULLNAME) \
	    -Wl,-soname,$(LIBDL_SHARED).$(MAJOR_VERSION) -Wl,--whole-archive \
	    $(LIBDL) $(TOPDIR)lib/$(SHARED_FULLNAME)
	install -d $(TOPDIR)lib
	rm -f $(TOPDIR)lib/$(LIBDL_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBDL_SHARED).$(MAJOR_VERSION)
	install -m 644 $(LIBDL_SHARED_FULLNAME) $(TOPDIR)lib;
	(cd $(TOPDIR)lib; ln -sf $(LIBDL_SHARED_FULLNAME) $(LIBDL_SHARED)); 
	(cd $(TOPDIR)lib; ln -sf $(LIBDL_SHARED_FULLNAME) $(LIBDL_SHARED).$(MAJOR_VERSION)); 

clean:
	$(RM) -f .depend $(LIBDL_SHARED)* $(LIBDL_SHARED_FULLNAME) core *.o *.a *.s *.i tmp_make foo *~

