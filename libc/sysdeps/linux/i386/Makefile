# Makefile for uCLibc
#
# Copyright (C) 2000 by Lineo, inc.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Library General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA 02111-1307 USA
#
# Derived in part from the Linux-8086 C library, the GNU C Library, and several
# other sundry sources.  Files within this library are copyright by their
# respective copyright holders.

TOPDIR=../../../
include $(TOPDIR)Rules.make
LIBC=$(TOPDIR)libc.a
ASFLAGS=$(CFLAGS)

MSRC=syscalls.S
include makefile.objs

SSRC=setjmp.S longjmp.S _start.S _exit.S #fork.S
SOBJS=$(patsubst %.S,%.o, $(SSRC))

CSRC=readdir.c #select.c
COBJS=$(patsubst %.c,%.o, $(CSRC))


all: $(MOBJ) $(SOBJS) $(COBJS) $(LIBC)

$(LIBC): $(MOBJ) $(SOBJS) $(COBJS)
	$(AR) $(ARFLAGS) $(LIBC) $(MOBJ) $(SOBJS) $(COBJS)

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o

$(SOBJS): $(SSRC)

$(COBJS): $(CSRC)


clean:
	rm -f *.[oa] *~ core

