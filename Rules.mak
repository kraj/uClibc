# Rules.make for uClibc
#
# This file contains rules which are shared between multiple Makefiles.  All
# normal configuration options live in the file named "Config".  You probably
# should not mess with this file unless you know what you are doing...  
# 
# Copyright (C) 2000 by Lineo, inc.
# Copyright (C) 2000-2002 Erik Andersen <andersen@uclibc.org>
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

-include $(TOPDIR)Config

# Be sure to update include/features.h when changing this...
MAJOR_VERSION:=0
MINOR_VERSION:=9
SUBLEVEL:=15
VERSION:=$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL)


LIBNAME:=libc.a
SHARED_FULLNAME:=libuClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so
SHARED_MAJORNAME:=libc.so.$(MAJOR_VERSION)
UCLIBC_LDSO:=ld-uClibc.so.$(MAJOR_VERSION)
LIBC:=$(TOPDIR)libc/libc.a
LIBGCC:=$(shell $(CC) $(LIBGCC_CFLAGS) -print-libgcc-file-name)

NATIVE_ARCH:= ${shell uname -m | sed \
		-e 's/i.86/i386/' \
		-e 's/sparc.*/sparc/' \
		-e 's/arm.*/arm/g' \
		-e 's/m68k.*/m68k/' \
		-e 's/ppc/powerpc/g' \
		-e 's/v850.*/v850/g' \
		-e 's/sh[234].*/sh/' \
		-e 's/mips.*/mips/' \
		}
ifeq ($(strip $(TARGET_ARCH)),)
TARGET_ARCH:=${shell $(CC) -dumpmachine | sed -e s'/-.*//' \
		-e 's/i.86/i386/' \
		-e 's/sparc.*/sparc/' \
		-e 's/arm.*/arm/g' \
		-e 's/m68k.*/m68k/' \
		-e 's/ppc/powerpc/g' \
		-e 's/v850.*/v850/g' \
		-e 's/sh[234]/sh/' \
		-e 's/mips.*/mips/' \
		-e 's/cris.*/cris/' \
		}
endif

# Ensure consistent filename sort order
LC_COLLATE      := C
export LC_COLLATE

ARFLAGS:=r

# Some nice architecture specific optimizations
ifndef OPTIMIZATION

# use '-Os' optimization if available, else use -O2, allow Config to override
OPTIMIZATION:= ${shell if $(CC) -Os -S -o /dev/null -xc /dev/null >/dev/null 2>&1; \
    then echo "-Os"; else echo "-O2" ; fi}
ifeq ($(strip $(TARGET_ARCH)),arm)
	OPTIMIZATION+=-fstrict-aliasing
endif
ifeq ($(strip $(TARGET_ARCH)),i386)
	OPTIMIZATION+=-march=i386
	OPTIMIZATION += ${shell if $(CC) -mpreferred-stack-boundary=2 -S -o /dev/null -xc \
		/dev/null >/dev/null 2>&1; then echo "-mpreferred-stack-boundary=2"; fi}
	OPTIMIZATION += ${shell if $(CC) -falign-functions=1 -falign-jumps=0 -falign-loops=0 \
		-S -o /dev/null -xc /dev/null >/dev/null 2>&1; then echo \
		"-falign-functions=1 -falign-jumps=0 -falign-loops=0"; else \
		if $(CC) -malign-functions=0 -malign-jumps=0 -S -o /dev/null -xc \
		/dev/null >/dev/null 2>&1; then echo "-malign-functions=0 -malign-jumps=0"; fi; fi}
endif
endif

# Add a bunch of extra pedantic annoyingly strict checks
WARNINGS+=-Wall -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing


CFLAGS:=$(WARNINGS) $(OPTIMIZATION) -fno-builtin -nostdinc $(CPUFLAGS) \
	-I$(TOPDIR)include -iwithprefix include -I. -D_LIBC $(ARCH_CFLAGS)
NATIVE_CFLAGS:=-O2 -Wall

ifeq ($(strip $(DODEBUG)),true)
    CFLAGS += -g
    LDFLAGS:= -shared --warn-common --warn-once -z combreloc
    STRIPTOOL:= true -Since_we_are_debugging
else
    CFLAGS  += -DNDEBUG #-fomit-frame-pointer
    LDFLAGS := -s -shared --warn-common --warn-once -z combreloc
endif

ifeq ($(strip $(HAVE_SHARED)),true)
    DOPIC:=true
    LIBRARY_CACHE:=#-DUSE_CACHE
    ifeq ($(strip $(BUILD_UCLIBC_LDSO)),true)
	LDSO:=$(TOPDIR)lib/$(UCLIBC_LDSO)
	DYNAMIC_LINKER:=$(SHARED_LIB_LOADER_PATH)/$(UCLIBC_LDSO)
	BUILD_DYNAMIC_LINKER:=${shell cd $(TOPDIR) && pwd}/lib/$(UCLIBC_LDSO)
    else
	LDSO:=$(SYSTEM_LDSO)
	BUILD_UCLIBC_LDSO:=false
	DYNAMIC_LINKER:=/lib/$(notdir $(SYSTEM_LDSO))
	BUILD_DYNAMIC_LINKER:=/lib/$(notdir $(SYSTEM_LDSO))
   endif
endif
ifeq ($(strip $(DOPIC)),true)
    CFLAGS += -fPIC
endif
# Currently locale support requires wide char support.
ifeq ($(strip $(HAS_LOCALE)),true)
    HAS_WCHAR:=true
endif

# TARGET_PREFIX is the directory under which which the uClibc runtime
# environment will be installed and used on the target system.   The 
# result will look something like the following:
#   TARGET_PREFIX/
#	lib/            <contains all runtime and static libs>
#	usr/lib/        <this directory is searched for runtime libs>
#	etc/            <weher the shared library cache and configuration 
#	                information go if you enabled LIBRARY_CACHE above>
# Very few people will need to change this value from the default...
TARGET_PREFIX = /

