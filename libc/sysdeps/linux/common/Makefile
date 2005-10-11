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

TOPDIR=../../../../
include $(TOPDIR)Rules.mak

SRCS  = $(wildcard *.c)

ifeq ($(strip $(EXCLUDE_BRK)),y)
SRCS := $(filter-out sbrk.c,$(SRCS))
endif

SRCS := $(filter-out ssp-local.c,$(SRCS))
ifneq ($(strip $(UCLIBC_HAS_SSP)),y)
SRCS := $(filter-out ssp.c,$(SRCS))
NONSHARED_OBJ_LIST=
else
NONSHARED_OBJ_LIST=../../../nonshared_obj.sysdeps.common
ssp.o: CFLAGS += $(SSP_DISABLE_FLAGS)
ssp-local.o: CFLAGS += $(SSP_DISABLE_FLAGS)
endif

OBJS  = $(patsubst %.c,%.o, $(SRCS))

NONSHARED_OBJS = ssp-local.o

OBJ_LIST=../../../obj.sysdeps.common

all: $(OBJ_LIST) $(NONSHARED_OBJ_LIST)

$(OBJ_LIST): $(OBJS)
	echo $(patsubst %, sysdeps/linux/common/%, $(OBJS)) > $(OBJ_LIST)

$(NONSHARED_OBJ_LIST): $(NONSHARED_OBJS)
	echo $(patsubst %, sysdeps/linux/common/%, $(NONSHARED_OBJS)) > $(NONSHARED_OBJ_LIST)

$(OBJS) $(NONSHARED_OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

headers:
	$(LN) -fs ../libc/sysdeps/linux/common/fpu_control.h $(TOPDIR)/include/

clean:
	$(RM) *.o *~ core
