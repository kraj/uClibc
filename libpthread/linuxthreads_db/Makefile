# Makefile for uClibc's pthread library
#
# Copyright (C) 2002 Erik Andersen <andersen@uclibc.org>
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

#CFLAGS+=$(SSP_ALL_CFLAGS)

LIB_NAME := libthread_db
AR_LIB_NAME := $(TOPDIR)lib/$(LIB_NAME).a
SO_LIB_NAME = $(TOPDIR)lib/$(LIB_NAME).so
SO_FULL_NAME = $(LIB_NAME)-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

# Get the thread include dependencies and shared object name
CFLAGS += $(PTINC) -DLIBPTHREAD_SO="\"libpthread.so.$(MAJOR_VERSION)\""

# Remove any -z defs since this lib will have undefined symbols
LDFLAGS = $(subst -z defs,,$(LDFLAGS)) --warn-unresolved-symbols

CSRC := $(wildcard *.c)

OBJS := $(patsubst %.c,%.o, $(CSRC))

ifeq ($(HAVE_SHARED),y)
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
	$(RM) $(TOPDIR)lib/$(SO_FULL_NAME) $(SO_LIB_NAME).1 $@
	$(LD) $(LDFLAGS) -soname=$(LIB_NAME).so.1 \
		-o $(TOPDIR)lib/$(SO_FULL_NAME) --whole-archive $< \
		--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
		-L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC)
	$(LN) -sf $(SO_FULL_NAME) $(SO_LIB_NAME).1
	$(LN) -sf $(SO_FULL_NAME) $@

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o *~ core
