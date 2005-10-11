# Makefile for uClibc
#
# Copyright (C) 2002-2003 Erik Andersen <andersen@uclibc.org>
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

CFLAGS+=$(SSP_ALL_CFLAGS)

LIB_NAME := libnsl
AR_LIB_NAME := $(TOPDIR)lib/$(LIB_NAME).a
SO_LIB_NAME = $(TOPDIR)lib/$(LIB_NAME).so
SO_FULL_NAME = $(LIB_NAME)-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

CSRC := $(wildcard *.c)

OBJS := $(patsubst %.c,%.o, $(CSRC))

ifeq ($(strip $(HAVE_SHARED)),y)
all: $(SO_LIB_NAME)
else
all: $(AR_LIB_NAME)
endif

$(AR_LIB_NAME): $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $@
	$(STRIPTOOL) -x -R .note -R .comment $^
	$(AR) $(ARFLAGS) $@ $^

$(SO_LIB_NAME): $(AR_LIB_NAME)
	$(RM) $(TOPDIR)lib/$(SO_FULL_NAME) $(SO_LIB_NAME).$(MAJOR_VERSION) $@
	$(LD) $(LDFLAGS) -soname=$(LIB_NAME).so.$(MAJOR_VERSION) \
		-o $(TOPDIR)lib/$(SO_FULL_NAME) --whole-archive $< \
		--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
		-L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC)
	$(LN) -sf $(SO_FULL_NAME) $(SO_LIB_NAME).$(MAJOR_VERSION)
	$(LN) -sf $(SO_FULL_NAME) $@

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o *~ core
