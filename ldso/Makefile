# Makefile for uClibc
#
# Copyright (C) 2000-2004 Erik Andersen <andersen@uclibc.org>
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

TOPDIR=../
include $(TOPDIR)Rules.mak

ALL_SUBDIRS = ldso libdl


all: headers
ifeq ($(strip $(BUILD_UCLIBC_LDSO)),y)
	$(MAKE) -C ldso;
else
	echo "Not building ld-uClibc"
endif

shared:
ifeq ($(strip $(BUILD_UCLIBC_LDSO)),y)
	$(MAKE) -C libdl;
else
	echo "Not building libdl"
endif

include/dl-progname.h: Makefile
	echo "const char *_dl_progname=\""$(UCLIBC_LDSO)"\";" \
		> include/dl-progname.h
	echo "#include \"$(TARGET_ARCH)/elfinterp.c\"" \
		>> include/dl-progname.h

headers: include/dl-progname.h
	$(LN) -fs $(TOPDIR)../include/elf.h include/
	$(LN) -fs ../ldso/$(TARGET_ARCH)/dl-startup.h include/
	$(LN) -fs ../ldso/$(TARGET_ARCH)/dl-syscalls.h include/
	$(LN) -fs ../ldso/$(TARGET_ARCH)/dl-sysdep.h include/

clean:
	set -e ; for d in $(ALL_SUBDIRS) ; do $(MAKE) -C $$d $@ ; done
	-find . -name '*~' | xargs $(RM)
	$(RM) include/elf.h include/dl-startup.h include/dl-syscalls.h \
		include/dl-sysdep.h include/dl-progname.h
