# Rules.make for uClibc
#
# This file contains rules which are shared between multiple Makefiles.  All
# normal configuration options live in the file named "Config".  You probably
# should not mess with this file unless you know what you are doing...  
#   -Erik Andersen <andersen@lineo.com> < andersee@debian.org>
# 
# Copyright (C) 2000, 2001 by Lineo, inc.
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

include $(TOPDIR)Config

MAJOR_VERSION=0
MINOR_VERSION=9.5

LIBNAME=libc.a
SHARED_FULLNAME=libuClibc-$(MAJOR_VERSION).$(MINOR_VERSION).so
SHARED_MAJORNAME=libc.so.$(MAJOR_VERSION)
UCLIBC_LDSO=ld-uClibc.so.$(MAJOR_VERSION)
LIBC=$(TOPDIR)libc/libc.a

BUILDTIME = $(shell TZ=UTC date --utc "+%Y.%m.%d-%H:%M%z")

GCCINCDIR = $(shell $(CC) -print-search-dirs | sed -ne "s/install: \(.*\)/\1include/gp")

# use '-Os' optimization if available, else use -O2, allow Config to override
ifndef OPTIMIZATION
OPTIMIZATION = $(shell if $(CC) -Os -S -o /dev/null -xc /dev/null >/dev/null 2>&1; \
    then echo "-Os"; else echo "-O2" ; fi)
endif

ARFLAGS=r

CCFLAGS=$(WARNINGS) $(OPTIMIZATION) -fno-builtin -nostdinc $(CPUFLAGS) -I$(TOPDIR)include -I$(GCCINCDIR) -I. -D_LIBC
TARGET_CCFLAGS=--uclibc-use-build-dir $(WARNINGS) $(OPTIMIZATION) $(CPUFLAGS)

CFLAGS=$(ARCH_CFLAGS) $(CCFLAGS) $(DEFS) $(ARCH_CFLAGS2)
TARGET_CC= $(TOPDIR)extra/gcc-uClibc/$(TARGET_ARCH)-uclibc-gcc
TARGET_CFLAGS=$(ARCH_CFLAGS) $(TARGET_CCFLAGS) $(DEFS) $(ARCH_CFLAGS2)
NATIVE_ARCH = $(shell uname -m | sed -e 's/i.86/i386/' -e 's/sparc.*/sparc/' \
		-e 's/arm.*/arm/g' -e 's/m68k.*/m68k/' -e 's/ppc/powerpc/g')

ifeq ($(strip $(DODEBUG)),true)
    CFLAGS += -g
    TARGET_CFLAGS += -g
    LDFLAGS = -nostdlib -Wl,-warn-common 
    TARGET_LDFLAGS = --uclibc-use-build-dir -Wl,-warn-common
    STRIPTOOL = /bin/true -Since_we_are_debugging
else
    CFLAGS  += -DNDEBUG #-fomit-frame-pointer
    TARGET_CFLAGS += -DNDEBUG #-fomit-frame-pointer
    LDFLAGS  = -s -nostdlib -Wl,-warn-common
    TARGET_LDFLAGS = --uclibc-use-build-dir -s -Wl,-warn-common
endif
ifeq ($(strip $(DOPIC)),true)
    CFLAGS += -fPIC
endif

# Disable libm if HAS_FLOATING_POINT isn't true.
ifneq ($(HAS_FLOATING_POINT),true)
	HAS_LIBM_FLOAT = false
	HAS_LIBM_DOUBLE = false
	HAS_LIBM_LONG_DOUBLE = false
endif

ifeq ($(strip $(HAVE_SHARED)),true)
    LIBRARY_CACHE=#-DUSE_CACHE
ifeq ($(strip $(BUILD_UCLIBC_LDSO)),true)
	LDSO=$(TOPDIR)lib/$(UCLIBC_LDSO)
	DYNAMIC_LINKER=$(SHARED_LIB_LOADER_PATH)/$(UCLIBC_LDSO)
	BUILD_DYNAMIC_LINKER=$(shell cd $(TOPDIR)lib; pwd)/$(UCLIBC_LDSO)
else
	LDSO=$(SYSTEM_LDSO)
	DYNAMIC_LINKER=/lib/$(notdir $(SYSTEM_LDSO))
	BUILD_DYNAMIC_LINKER=/lib/$(notdir $(SYSTEM_LDSO))
endif
endif


