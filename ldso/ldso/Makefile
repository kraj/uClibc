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

TOPDIR=../../
include $(TOPDIR)Rules.mak

XXFLAGS=$(XWARNINGS) $(SSP_DISABLE_FLAGS)

ifeq ($(DODEBUG),y)
# Not really much point in including debugging info, since gdb
# can't really debug ldso, since gdb requires help from ldso to
# debug things....
XXFLAGS+=-Os -g
else
XXFLAGS+=$(OPTIMIZATION)
endif
XXFLAGS+= -DUCLIBC_LDSO=\"$(UCLIBC_LDSO)\" $(XARCH_CFLAGS) $(CPU_CFLAGS) $(PICFLAG) \
	-DUCLIBC_RUNTIME_PREFIX=\"$(RUNTIME_PREFIX)\" \
	-fno-builtin -nostdinc -D_LIBC -I$(TOPDIR)ldso/include -I. -I$(TOPDIR)include

XXFLAGS+=-isystem $(shell $(CC) -print-file-name=include)

ifeq ($(SUPPORT_LD_DEBUG),y)
XXFLAGS+=-D__SUPPORT_LD_DEBUG__
endif

ifeq ($(SUPPORT_LD_DEBUG_EARLY),y)
XXFLAGS+=-D__SUPPORT_LD_DEBUG_EARLY__
endif

#This stuff will not work with -fomit-frame-pointer
XXFLAGS := $(XXFLAGS:-fomit-frame-pointer=)

# BEWARE!!! At least mips* will die if -O0 is used!!!
ifeq ($(strip $(TARGET_ARCH)),mips)
XXFLAGS := $(XXFLAGS:-O0=-O1)
endif

ASFLAGS=$(XXFLAGS)
ifeq ($(UCLIBC_BUILD_NOEXECSTACK),y)
ASFLAGS+=$(call check_as,--noexecstack)
endif

LDFLAGS=$(CPU_LDFLAGS-y) -e _start -z now -Bsymbolic -shared --warn-common --export-dynamic --sort-common \
	-z combreloc --discard-locals --discard-all --no-undefined

ifeq ($(UCLIBC_BUILD_RELRO),y)
LDFLAGS+=-z relro
endif

ifneq ($(SUPPORT_LD_DEBUG),y)
LDFLAGS+=-s
endif

LIB_NAME := ld-uClibc
SO_LIB_NAME := $(TOPDIR)lib/$(UCLIBC_LDSO)
SO_FULL_NAME := $(LIB_NAME)-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

CSRC := ldso.c
COBJ := $(patsubst %.c,%.o, $(CSRC))

SSRC := $(wildcard $(TARGET_ARCH)/*.S)
SOBJ := $(patsubst %.S,%.o, $(SSRC))

OBJS := $(SOBJ) $(COBJ)

all: $(SO_LIB_NAME)

$(SO_LIB_NAME): $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(SO_FULL_NAME) $@
	$(STRIPTOOL) -x -R .note -R .comment $^
	$(LD) $(LDFLAGS) -soname=$(UCLIBC_LDSO) \
		-o $(TOPDIR)lib/$(SO_FULL_NAME) $^ $(LIBGCC)
	$(LN) -sf $(SO_FULL_NAME) $@

$(COBJ): %.o : %.c
	$(CC) $(XXFLAGS) -c $< -o $@

$(SOBJ): %.o : %.S
	$(CC) $(ASFLAGS) -c $< -o $@

clean:
	$(RM) *.o */*.o *~ core
