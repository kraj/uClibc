# Makefile for uClibc
#
# Copyright (C) 2000 by by Lineo, inc. and Erik Andersen
# Copyright (C) 2000, 2001 by Erik Andersen <andersee@debian.org>
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

LIBPTHREAD=libpthread.a
LIBPTHREAD_SHARED=libpthread.so
LIBPTHREAD_SHARED_FULLNAME=libpthread-$(MAJOR_VERSION).$(MINOR_VERSION).so

CSRC = pthread.c
OBJS=$(patsubst %.c,%.o, $(CSRC))

all: $(OBJS) $(LIBPTHREAD)

$(LIBPTHREAD): ar-target

ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBPTHREAD) $(OBJS)
	install -d $(TOPDIR)lib
	rm -f $(TOPDIR)lib/$(LIBPTHREAD)
	install -m 644 $(LIBPTHREAD) $(TOPDIR)lib

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(OBJ): Makefile

shared: all
	$(LD) $(LDFLAGS) -o $(LIBPTHREAD_SHARED_FULLNAME) \
	    -soname=$(LIBPTHREAD_SHARED).$(MAJOR_VERSION) \
	    --whole-archive $(LIBPTHREAD);
	install -d $(TOPDIR)lib
	rm -f $(TOPDIR)lib/$(LIBPTHREAD_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBPTHREAD_SHARED).$(MAJOR_VERSION)
	install -m 644 $(LIBPTHREAD_SHARED_FULLNAME) $(TOPDIR)lib;
	(cd $(TOPDIR)lib && ln -sf $(LIBPTHREAD_SHARED_FULLNAME) $(LIBPTHREAD_SHARED)); 
	(cd $(TOPDIR)lib && ln -sf $(LIBPTHREAD_SHARED_FULLNAME) $(LIBPTHREAD_SHARED).$(MAJOR_VERSION)); 

clean: 
	rm -f *.[oa] *~ core $(LIBPTHREAD_SHARED)* $(LIBPTHREAD_SHARED_FULLNAME)*

