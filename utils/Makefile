# Makefile for uClibc
#
# Copyright (C) 2000-2003 Erik Andersen <andersen@uclibc.org>
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

# Pull in the user's uClibc configuration, but do not
# pull in Rules.mak.....
TOPDIR=../
include $(TOPDIR).config

MAJOR_VERSION=0
UCLIBC_LDSO=ld-uClibc.so.$(MAJOR_VERSION)

# A nifty macro to make testing gcc features easier
check_gcc=$(shell if $(CC) $(1) -S -o /dev/null -xc /dev/null > /dev/null 2>&1; \
	then echo "$(1)"; else echo "$(2)"; fi)

# use '-Os' optimization if available, else use -O2, allow Config to override
OPTIMIZATION=$(call check_gcc,-Os,-O2)
UWARNINGS=$(subst ",, $(strip $(WARNINGS))) -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing
XARCH_CFLAGS=$(subst ",, $(strip $(ARCH_CFLAGS)))

ifndef CROSS
CROSS=
endif
CC= $(CROSS)gcc
AR= $(CROSS)ar
LD= $(CROSS)ld
NM= $(CROSS)nm
STRIPTOOL= $(CROSS)strip

INSTALL= install
LN= ln
RM= rm -f
override CFLAGS=$(UWARNINGS) $(OPTIMIZATION) #$(XARCH_CFLAGS)
override LDFLAGS=-s
ifeq ($(DODEBUG),y)
    override CFLAGS=$(UWARNINGS) -O0 -g3 #$(XARCH_CFLAGS)
    override LDFLAGS=
endif

# Make certain these contain a final "/", but no "//"s.
RUNTIME_PREFIX:=$(strip $(subst //,/, $(subst ,/, $(subst ",, $(strip $(RUNTIME_PREFIX))))))
DEVEL_PREFIX:=$(strip $(subst //,/, $(subst ,/, $(subst ",, $(strip $(DEVEL_PREFIX))))))


TARGETS = ldd ldconfig readelf

ifeq ($(HAVE_SHARED),y)
    LIBRARY_CACHE=#-DUSE_CACHE
    ifeq ($(BUILD_UCLIBC_LDSO),y)
	LDSO=$(TOPDIR)lib/$(UCLIBC_LDSO)
	DYNAMIC_LINKER=$(SHARED_LIB_LOADER_PREFIX)/$(UCLIBC_LDSO)
    else
	LDSO=$(SYSTEM_LDSO)
	DYNAMIC_LINKER=/lib/$(strip $(subst ",, $(notdir $(SYSTEM_LDSO))))
   endif
endif


XXFLAGS=
ifeq ($(strip $(LDSO_LDD_SUPPORT)),y)
XXFLAGS= -D__LDSO_LDD_SUPPORT
else
XXFLAGS=
endif

LDADD_LIBFLOAT=
ifeq ($(strip $(UCLIBC_HAS_SOFT_FLOAT)),y)
# If -msoft-float isn't supported, we want an error anyway.
    override CFLAGS += -msoft-float
ifeq ($(strip $(subst ",, $(strip $(TARGET_ARCH)))),arm)
    LDADD_LIBFLOAT=-lfloat
endif
endif

ifeq ($(strip $(HAVE_SHARED)),y)
all:	$(TARGETS)
else
all:
endif

headers:
	$(LN) -fs $(TOPDIR)include/elf.h

readelf: readelf.c
	$(CC) $(CFLAGS) -Wl,-s $^ -o $@ $(LDADD_LIBFLOAT)
	$(STRIPTOOL) -x -R .note -R .comment $@

ldconfig:	ldconfig.c readsoname.c
	$(CC) $(CFLAGS) $(XXFLAGS) -Wl,-s \
		-DUCLIBC_RUNTIME_PREFIX=$(R_PREFIX) \
		-DUCLIBC_LDSO=$(UCLIBC_LDSO) -I. -I../ldso/include \
		$^ -o $@ $(LDADD_LIBFLOAT)
	$(STRIPTOOL) -x -R .note -R .comment $@

ldd:	ldd.c
	$(CC) $(CFLAGS) $(XXFLAGS) -Wl,-s \
		-DUCLIBC_RUNTIME_PREFIX=$(R_PREFIX) \
		-DUCLIBC_LDSO=$(UCLIBC_LDSO) \
		$^ -o $@ $(LDADD_LIBFLOAT)
	$(STRIPTOOL) -x -R .note -R .comment $@

clean:
	$(RM) $(TARGETS) *.o *~ core *.target elf.h


readelf.c readsoname.c ldconfig.c ldd.c:	headers

install: all
ifeq ($(strip $(HAVE_SHARED)),y)
	$(INSTALL) -d $(PREFIX)$(RUNTIME_PREFIX)sbin
	$(INSTALL) -d $(PREFIX)$(RUNTIME_PREFIX)usr/bin
	$(INSTALL) -m 755 ldd $(PREFIX)$(RUNTIME_PREFIX)usr/bin/ldd
	$(INSTALL) -m 755 ldconfig $(PREFIX)$(RUNTIME_PREFIX)sbin/ldconfig;
	# For now, don't bother with readelf since surely the host
	# system has binutils, or we couldn't have gotten this far...
	#$(INSTALL) -m 755 readelf $(PREFIX)$(RUNTIME_PREFIX)usr/bin/readelf
endif

