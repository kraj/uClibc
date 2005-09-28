# Makefile for uClibc's math library
#
# Copyright (C) 2000,2001 Erik Andersen <andersen@uclibc.org>
#
# The routines included in this math library are derived from the
# math library for Apple's MacOS X/Darwin math library, which was
# itself swiped from FreeBSD.  The original copyright information
# is as follows:
#
#     Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
#
#     Developed at SunPro, a Sun Microsystems, Inc. business.
#     Permission to use, copy, modify, and distribute this
#     software is freely granted, provided that this notice
#     is preserved.
#
# It has been ported to work with uClibc and generally behave
# by Erik Andersen <andersen@codepoet.org>
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

CFLAGS+=-D_IEEE_LIBM -D_ISOC99_SOURCE -D_SVID_SOURCE

LIB_NAME=libm
AR_LIB_NAME=$(TOPDIR)lib/$(LIB_NAME).a

ifeq ($(strip $(DO_C99_MATH)),y)
CSRC = $(wildcard *.c)
else
CSRC =
endif

OBJS=$(patsubst %.c,%.o, $(CSRC))

all: add_to_archive

add_to_archive: $(OBJS)
	$(AR) $(ARFLAGS) $(AR_LIB_NAME) $(OBJS)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

tags:
	ctags -R

clean:
	$(RM) *.o *~ core
