# Makefile for uClibc
#
# Copyright (C) 2000 by Lineo, inc.
# Copyright (C) 2000,2001 Erik Andersen <andersen@uclibc.org>
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
# Derived in part from the Linux-8086 C library, the GNU C Library, and several
# other sundry sources.  Files within this library are copyright by their
# respective copyright holders.

TOPDIR=../../../../
include $(TOPDIR)Rules.mak
ASFLAGS=$(CFLAGS)

# If you're looking for vfork(), it is defined in include/unistd.h


ifeq ($(HAS_MMU),true)
CRT0=crt0.c
CRT0_OBJ=$(patsubst %.c,%.o, $(CRT0))
else
CRT0=crt0.S
CRT0_OBJ=$(patsubst %.S,%.o, $(CRT0))
endif

SSRC=setjmp.S vfork.S # longjmp.S _start.S clone.S
SOBJS=$(patsubst %.S,%.o, $(SSRC))

CSRC=ptrace.c
COBJS=$(patsubst %.c,%.o, $(CSRC))

OBJS=$(SOBJS) $(MOBJ) $(COBJS)



all: $(OBJS) $(LIBC)

$(LIBC): ar-target 

ar-target: $(OBJS) $(CRT0_OBJ)
	$(AR) $(ARFLAGS) $(LIBC) $(OBJS)
	cp $(CRT0_OBJ) $(TOPDIR)libc/$(CRT0_OBJ)

ifeq ($(HAS_MMU),true)
$(CRT0_OBJ): %.o : %.c
else
$(CRT0_OBJ): %.o : %.S
endif
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(SOBJS): %.o : %.S
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

headers:
	@(TOPDIR=$(TOPDIR) CC=$(CC) /bin/sh $(TOPDIR)/extra/scripts/gen_bits_syscall_h.sh > bits/syscall.h ) 
	cd $(TOPDIR)/include/sys && ln -fs ../../libc/sysdeps/linux/m68k/reg.h .
ifeq ($(strip $(HAVE_ELF)),false)
	echo "Working around compiler bug in the m68k-pic-coff toolchain"
	cd $(TOPDIR)/include && ln -fs ../libc/sysdeps/linux/m68k/float.h .
endif


clean:
	rm -f *.[oa] *~ core
	rm -f bits/syscall.h
ifeq ($(strip $(HAVE_ELF)),false)
	rm -f $(TOPDIR)/include/float.h
endif

