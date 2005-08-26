# Makefile for uClibc
#
# Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
#
# Licensed under the GNU Library General Public License version 2 or later.
# See the COPYING.LIB file in the toplevel for more information.

TOPDIR=../../../
include $(TOPDIR)Rules.mak

CSRCS = $(wildcard *.c)
COBJS = $(patsubst %.c,%.o,$(CSRCS))

SSRCS = $(wildcard *.S)
SOBJS = $(patsubst %.S,%.o,$(SSRCS))

OBJS = $(COBJS) $(SOBJS)

OBJ_LIST = ../../obj.string.$(TARGET_ARCH)

all: $(OBJ_LIST)

$(OBJ_LIST): $(OBJS)
	echo $(patsubst %, string/$(TARGET_ARCH)/%, $(OBJS)) > $(OBJ_LIST)

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

clean:
	$(RM) *.[oa] *~ core
