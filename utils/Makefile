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
# include Rules.mak.....
TOPDIR=../
include $(TOPDIR).config

# A nifty macro to make testing gcc features easier
check_gcc=$(shell if $(CC) $(1) -S -o /dev/null -xc /dev/null > /dev/null 2>&1; \
	then echo "$(1)"; else echo "$(2)"; fi)

# use '-Os' optimization if available, else use -O2, allow Config to override
OPTIMIZATION+=$(call check_gcc,-Os,-O2)
XWARNINGS=$(subst ",, $(strip $(WARNINGS))) -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing

MAJOR_VERSION:=0
UCLIBC_LDSO:=ld-uClibc.so.$(MAJOR_VERSION)

ifndef CROSS
CROSS=
endif
CC= $(CROSS)gcc
AR= $(CROSS)ar
LD= $(CROSS)ld
NM= $(CROSS)nm
STRIP= $(CROSS)strip
LN=ln

ifeq ($(DODEBUG),y)
    CFLAGS = $(XWARNINGS) -O0 -g3
    LDFLAGS =
else
    CFLAGS=$(XWARNINGS) $(OPTIMIZATION)
    LDFLAGS = -s
endif

# Make certain these contain a final "/", but no "//"s.
RUNTIME_PREFIX:=$(strip $(subst //,/, $(subst ,/, $(subst ",, $(strip $(RUNTIME_PREFIX))))))
DEVEL_PREFIX:=$(strip $(subst //,/, $(subst ,/, $(subst ",, $(strip $(DEVEL_PREFIX))))))
export RUNTIME_PREFIX DEVEL_PREFIX


TARGETS = ldd ldconfig readelf

ifeq ($(strip $(LDSO_LDD_SUPPORT)),y)
XXFLAGS = -D__LDSO_LDD_SUPPORT
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
	$(STRIP) -x -R .note -R .comment $@

ldconfig:	ldconfig.c readsoname.c
	$(CC) $(CFLAGS) $(XXFLAGS) -Wl,-s \
		-DUCLIBC_RUNTIME_PREFIX=$(R_PREFIX) \
		-DUCLIBC_LDSO=$(UCLIBC_LDSO) -I. -I../ldso/include \
		$^ -o $@ $(LDADD_LIBFLOAT)
	$(STRIP) -x -R .note -R .comment $@

ldd:	ldd.c
	$(CC) $(CFLAGS) $(XXFLAGS) -Wl,-s \
		-DUCLIBC_RUNTIME_PREFIX=$(R_PREFIX) \
		-DUCLIBC_LDSO=$(UCLIBC_LDSO) \
		$^ -o $@ $(LDADD_LIBFLOAT)
	$(STRIP) -x -R .note -R .comment $@

clean:
	$(RM) $(TARGETS) *.o *~ core *.target elf.h

readelf.c readsoname.c ldconfig.c ldd.c:	headers
