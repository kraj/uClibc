# Makefile for uClibc
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


TOPDIR=../../
include $(TOPDIR)Rules.mak

XXFLAGS=$(XWARNINGS) $(SSP_CFLAGS) $(SSP_ALL_CFLAGS)
ifeq ($(DODEBUG),y)
XXFLAGS+=-O0 -g3
else
XXFLAGS+=$(OPTIMIZATION)
endif
XXFLAGS+= $(XARCH_CFLAGS) $(CPU_CFLAGS) \
	-DUCLIBC_RUNTIME_PREFIX=\"$(RUNTIME_PREFIX)\" \
	-fno-builtin -nostdinc -D_LIBC -I$(TOPDIR)ldso/include -I$(TOPDIR)ldso/ldso -I$(TOPDIR)include

XXFLAGS+=-isystem $(shell $(CC) -print-file-name=include)
ifeq ($(SUPPORT_LD_DEBUG),y)
XXFLAGS+=-D__SUPPORT_LD_DEBUG__
endif
XXFLAGS_NOPIC:=$(XXFLAGS)
ifeq ($(DOPIC),y)
    XXFLAGS += $(PICFLAG) -D__LIBDL_SHARED__
endif

# BEWARE!!! At least mips* will die if -O0 is used!!!
ifeq ($(TARGET_ARCH),mips)
XXFLAGS := $(XXFLAGS:-O0=-O1)
endif

LIB_NAME := libdl
AR_LIB_NAME := $(TOPDIR)lib/$(LIB_NAME).a
SO_LIB_NAME := $(TOPDIR)lib/$(LIB_NAME).so
SO_FULL_NAME := $(LIB_NAME)-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

CSRC := libdl.c
OBJS := libdl.o
PIC_OBJS := libdl_pic.o

# we do not get here, do we need a libdl.a, if ld.so is not ours?
#ifeq ($(HAVE_SHARED),y)
all: $(AR_LIB_NAME) $(SO_LIB_NAME)
#else
#all: $(AR_LIB_NAME)
#endif

$(AR_LIB_NAME): $(OBJS) ../ldso/$(TARGET_ARCH)/resolve.o
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $@
	$(AR) $(ARFLAGS) $@ $^

shared_$(LIB_NAME).a: $(PIC_OBJS)
	$(RM) $@
	$(STRIPTOOL) -x -R .note -R .comment $^
	$(AR) $(ARFLAGS) $@ $^

# this will be built only on the second round, when libc.so is present,
# else we would link against libc.a
$(SO_LIB_NAME): shared_$(LIB_NAME).a
	if [ -f $(TOPDIR)lib/libc.so ] ; then \
		$(RM) $(TOPDIR)lib/$(SO_FULL_NAME) $(SO_LIB_NAME).$(MAJOR_VERSION) $@ ; \
		$(LD) $(LDFLAGS) -soname=$(LIB_NAME).so.$(MAJOR_VERSION) \
			-o $(TOPDIR)lib/$(SO_FULL_NAME) -fini dl_cleanup --whole-archive $< \
			--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
			-L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(TOPDIR)lib/$(UCLIBC_LDSO) $(LIBGCC) ; \
		$(LN) -sf $(SO_FULL_NAME) $(SO_LIB_NAME).$(MAJOR_VERSION) ; \
		$(LN) -sf $(SO_FULL_NAME) $@ ; \
	fi

$(OBJS): %.o : %.c
	$(CC) $(XXFLAGS_NOPIC) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

libdl_pic.o: libdl.c
	$(CC) $(XXFLAGS) -c $< -o $@

clean:
	$(RM) *.[oa] *~ core
