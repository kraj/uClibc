# Makefile for uClibc
#
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

TOPDIR=../../
include $(TOPDIR)Rules.mak
TARGET_CC = $(TOPDIR)extra/gcc-uClibc/$(TARGET_ARCH)-uclibc-gcc

TARGETS = ldd ldconfig
ifeq ($(OSTYPE),linux)
TARGETS += readelf
endif
#ifneq ($(strip $(LIBRARY_CACHE)),)
#TARGETS += ldconfig
#endif

ifeq ($(strip $(LDSO_LDD_SUPPORT)),y)
XXFLAGS = -D__LDSO_LDD_SUPPORT
endif

all:	$(TARGETS)

headers:
	$(LN) -fs $(TOPDIR)include/elf.h

readelf:	readelf.c
	$(HOSTCC) $(HOSTCFLAGS) -I. -I../include $^ -o $@
	strip -x -R .note -R .comment $@

readelf.target: readelf.c
	$(TARGET_CC) $(CFLAGS) -Wl,-s $^ -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@

readsoname.o:	readsoname.c readsoname2.c
	$(HOSTCC) $(HOSTCFLAGS) -I. -I../include -c $< -o $@
	strip -x -R .note -R .comment $*.o

#ldconfig.o:	ldconfig.c
#	$(HOSTCC) $(HOSTCFLAGS) -I. -I../include \
#		-DUCLIBC_TARGET_PREFIX=\"$(TARGET_PREFIX)\" -c $< -o $@
#	strip -x -R .note -R .comment $*.o

#ldconfig:	ldconfig.o readsoname.o
#	$(HOSTCC) $(HOSTCFLAGS) $^ -o $@
#	strip -x -R .note -R .comment $@

ldconfig:	ldconfig.c readsoname.c
	$(HOSTCC) $(HOSTCFLAGS) $(XXFLAGS) -DUCLIBC_TARGET_PREFIX=\"$(TARGET_PREFIX)\" \
		-DUCLIBC_DEVEL_PREFIX=\"$(DEVEL_PREFIX)\" \
		-DUCLIBC_BUILD_DIR=\"$(shell cd $(TOPDIR) && pwd)\" \
		-DUCLIBC_LDSO=\"$(UCLIBC_LDSO)\" -I. -I../include \
		$^ -o $@
	strip -x -R .note -R .comment $@

ldconfig.target:	ldconfig.c readsoname.c
	$(TARGET_CC) $(CFLAGS) $(XXFLAGS) -Wl,-s -DUCLIBC_TARGET_PREFIX=\"$(TARGET_PREFIX)\" \
		-DUCLIBC_DEVEL_PREFIX=\"$(DEVEL_PREFIX)\" \
		-DUCLIBC_BUILD_DIR=\"$(shell cd $(TOPDIR) && pwd)\" \
		-DUCLIBC_LDSO=\"$(UCLIBC_LDSO)\" -I. -I../include \
		$^ -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@

ldd:	ldd.c
	$(HOSTCC) $(HOSTCFLAGS) $(XXFLAGS) -DUCLIBC_TARGET_PREFIX=\"$(TARGET_PREFIX)\" \
		-DUCLIBC_DEVEL_PREFIX=\"$(DEVEL_PREFIX)\" \
		-DUCLIBC_BUILD_DIR=\"$(shell cd $(TOPDIR) && pwd)\" \
		-DUCLIBC_LDSO=\"$(UCLIBC_LDSO)\" -I. -I../include \
		$^ -o $@
	strip -x -R .note -R .comment $@

ldd.target:	ldd.c
	$(TARGET_CC) $(CFLAGS) $(XXFLAGS) -Wl,-s -DUCLIBC_TARGET_PREFIX=\"$(TARGET_PREFIX)\" \
		-DUCLIBC_DEVEL_PREFIX=\"$(DEVEL_PREFIX)\" \
		-DUCLIBC_BUILD_DIR=\"$(shell cd $(TOPDIR) && pwd)\" \
		-DUCLIBC_LDSO=\"$(UCLIBC_LDSO)\" \
		$^ -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@

clean:
	$(RM) $(TARGETS) *.o *~ core *.target elf.h

readelf.c readsoname.c ldconfig.c ldd.c:	headers
