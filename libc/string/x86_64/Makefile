# Makefile for uClibc
#
# Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
#
# Licensed under the GNU Library General Public License version 2 or later.
# See the COPYING.LIB file in the toplevel for more information.

TOPDIR=../../../
include $(TOPDIR)Rules.mak

CSRC := $(wildcard *.c)
COBJ := $(patsubst %.c,%.o,$(CSRC))

SSRC := $(wildcard *.S)
SOBJ := $(patsubst %.S,%.o,$(SSRC))

OBJS := $(COBJ) $(SOBJ)

OBJ_LIST := ../../obj.string.$(TARGET_ARCH)

all: $(OBJ_LIST)

$(OBJ_LIST): $(OBJS)
	$(STRIPTOOL) -x -R .note -R .comment $^
	echo $(patsubst %, string/$(TARGET_ARCH)/%, $^) > $@

$(COBJ): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SOBJ): %.o : %.S
	$(CC) $(ASFLAGS) -c $< -o $@

clean:
	$(RM) *.o *~ core
