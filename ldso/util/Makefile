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

TARGETS=ldd.uclibc ldd readelf ldconfig
all: $(TARGETS)

readsoname.o: readsoname.c readsoname2.c
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o
    
ldconfig.o: ldconfig.c
	$(TARGET_CC) $(TARGET_CFLAGS) -DUCLIBC_TARGET_PREFIX=\"$(TARGET_PREFIX)\" \
		-c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

readelf: readelf.c
	$(TARGET_CC) $(TARGET_CFLAGS) -static readelf.c -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@

ifeq ($(strip $(LIBRARY_CACHE)),)
ldconfig: 
	echo "LIBRARY_CACHE disabled -- not building ldconfig"
else
ldconfig: ldconfig.o readsoname.o
	$(TARGET_CC) $(TARGET_CFLAGS) -static $^ -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
endif

ldd: ldd.c
	$(TARGET_CC) $(TARGET_CFLAGS) -DUCLIBC_TARGET_PREFIX=\"$(TARGET_PREFIX)\" \
		-DUCLIBC_DEVEL_PREFIX=\"$(DEVEL_PREFIX)\" \
		-DUCLIBC_BUILD_DIR=\"$(shell cd $(TOPDIR) && pwd)\" \
		-DUCLIBC_LDSO=\"$(UCLIBC_LDSO)\" \
		-static ldd.c -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@

ldd.uclibc: ldd.c
	$(NATIVE_CC) $(NATIVE_CFLAGS) -g -DUCLIBC_TARGET_PREFIX=\"$(TARGET_PREFIX)\" \
		-DUCLIBC_DEVEL_PREFIX=\"$(DEVEL_PREFIX)\" \
		-DUCLIBC_BUILD_DIR=\"$(shell cd $(TOPDIR) && pwd)\" \
		-DUCLIBC_LDSO=\"$(UCLIBC_LDSO)\" \
		ldd.c -o $@ 
	$(STRIPTOOL) -x -R .note -R .comment $@

clean:
	rm -f $(TARGETS) *.o *~ core

