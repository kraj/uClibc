# Makefile for uClibc's pthread library
#
# Copyright (C) 2002 Erik Andersen <andersen@uclibc.org>
# Copyright (C) 2003 Paul Mundt <lethal@linux-sh.org>
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

CFLAGS+=$(SSP_ALL_CFLAGS)

# This stuff will not compile without at least -O1
CFLAGS :=$(CFLAGS:-O0=-O1)

# Get the thread include dependencies
CFLAGS += $(PTINC)

ARCH_CFLAGS := $(CFLAGS)

ifeq ($(UCLIBC_CTOR_DTOR),y)
SHARED_START_FILES = $(TOPDIR)lib/crti.o $(LIBGCC_DIR)crtbeginS.o
SHARED_END_FILES = $(LIBGCC_DIR)crtendS.o $(TOPDIR)lib/crtn.o
endif

LIB_NAME := libpthread
AR_LIB_NAME := $(TOPDIR)lib/$(LIB_NAME).a
SO_LIB_NAME = $(TOPDIR)lib/$(LIB_NAME).so
SO_FULL_NAME = $(LIB_NAME)-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

ARCH_CSRC := $(wildcard sysdeps/$(TARGET_ARCH)/*.c)
ARCH_OBJS := $(patsubst %.c,%.o, $(ARCH_CSRC))
-include sysdeps/$(TARGET_ARCH)/Makefile.arch

CSRC = $(wildcard *.c)
ifneq ($(UCLIBC_HAS_XLOCALE),y)
CSRC := $(filter-out locale.c,$(CSRC))
endif

OBJS := $(patsubst %.c,%.o, $(CSRC))

ifeq ($(HAVE_SHARED),y)
all: $(SO_LIB_NAME)
else
all: $(AR_LIB_NAME)
endif

$(AR_LIB_NAME): $(OBJS) $(ARCH_OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $@
ifeq ($(PTHREADS_DEBUG_SUPPORT),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment $^
else
	$(STRIPTOOL) -x -R .note -R .comment $^
endif
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(AR) $(ARFLAGS) $@ $(ARCH_OBJS)

$(SO_LIB_NAME): $(AR_LIB_NAME)
	$(RM) $(TOPDIR)lib/$(SO_FULL_NAME) $(SO_LIB_NAME).$(MAJOR_VERSION) $@
	$(LD) $(LDFLAGS_NOSTRIP) -soname=$(LIB_NAME).so.$(MAJOR_VERSION) \
		-o $(TOPDIR)lib/$(SO_FULL_NAME) $(SHARED_START_FILES) --whole-archive $< \
		--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
		-L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC) $(SHARED_END_FILES)
	$(LN) -sf $(SO_FULL_NAME) $(SO_LIB_NAME).$(MAJOR_VERSION)
	$(LN) -sf $(SO_FULL_NAME) $@

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ARCH_OBJS): %.o : %.c
	$(CC) $(ARCH_CFLAGS) -c $< -o $@

clean:
	$(RM) *.o sysdeps/*/*.o *~ core
