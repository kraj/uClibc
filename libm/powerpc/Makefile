# Makefile for uClibc's math library
# Copyright (C) 2001 by Lineo, inc.
#
# This math library is derived primarily from the Cephes Math Library,
# copyright by Stephen L. Moshier <moshier@world.std.com>
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

TOPDIR=../../
include $(TOPDIR)Rules.mak

LIBM=../libm.a
TARGET_CC= $(TOPDIR)extra/gcc-uClibc/$(TARGET_ARCH)-uclibc-gcc
TARGET_CFLAGS+=-D_IEEE_LIBM -D_ISOC99_SOURCE -D_SVID_SOURCE

ifeq ($(strip $(DO_C99_MATH)),true)
CSRC =   ceilfloor.c  frexpldexp.c  logb.c  rndint.c  scalb.c  sign.c
else
CSRC =
endif
COBJS=$(patsubst %.c,%.o, $(CSRC))
OBJS=$(COBJS)


ifneq ($(strip $(HAS_FLOATING_POINT)),true)
all:  clean
else
all: $(OBJS) $(LIBM)
endif

$(LIBM): ar-target

ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBM) $(OBJS)

$(COBJS): %.o : %.c
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(OBJ): Makefile

tags:
	ctags -R

clean: 
	rm -f *.[oa] *~ core

