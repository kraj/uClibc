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

LIBUTIL=libutil.a
LIBUTIL_SHARED=libutil.so
TARGET_CC= $(TOPDIR)extra/gcc-uClibc/$(TARGET_ARCH)-uclibc-gcc

CSRC=forkpty.c login.c login_tty.c logout.c logwtmp.c openpty.c
COBJS=$(patsubst %.c,%.o, $(CSRC))

OBJS=$(COBJS)

all: $(OBJS) $(LIBC)

$(LIBC): ar-target

ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBUTIL) $(OBJS)

$(COBJS): %.o : %.c
	$(TARGET_CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

shared: $(LIBUTIL)
	$(TARGET_CC) $(LDFLAGS) -shared -o $(LIBUTIL_SHARED).$(MAJOR_VERSION) \
	   -Wl,-soname,$(LIBUTIL_SHARED).$(MAJOR_VERSION) $(COBJS) $(TOPDIR)$(SHARED_FULLNAME)

install: all
	install -d $(INSTALL_DIR)/lib
	install -m 644 $(LIBUTIL) $(INSTALL_DIR)/lib/
	@if [ -f $(LIBUTIL_SHARED).$(MAJOR_VERSION) ] ; then \
		install -m 644 $(LIBUTIL_SHARED).$(MAJOR_VERSION) $(INSTALL_DIR)/lib/; \
		(cd $(INSTALL_DIR)/lib/;ln -sf $(LIBUTIL_SHARED).$(MAJOR_VERSION) $(LIBUTIL_SHARED)); \
	fi;

clean:
	rm -f *.[oa] *~ core $(LIBUTIL_SHARED).$(MAJOR_VERSION)

