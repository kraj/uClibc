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
ASFLAGS=$(CFLAGS)

CRT0_SRC = crt0.S
CRT0_OBJ = crt0.o crt1.o # gcrt1.o
SCRT0_OBJ = $(patsubst %,S%, $(CRT0_OBJ))
CRT0_DEPS=gmon-start.S

CTOR_TARGETS = crti.o crtn.o

SSRC=__longjmp.S setjmp.S clone.S vfork.S
ifeq ($(strip $(UCLIBC_PROFILING)),y)
SSRC+=mcount.S
endif
SOBJS=$(patsubst %.S,%.o, $(SSRC))

CSRC=mmap.c sysdep.c brk.c sbrk.c __init_brk.c dl-iterate-phdr.c
COBJS=$(patsubst %.c,%.o, $(CSRC))

OBJS=$(SOBJS) $(COBJS)

all: $(OBJS) $(LIBC)

$(LIBC): ar-target 

ar-target: $(OBJS) $(CRT0_OBJ) $(SCRT0_OBJ) $(CTOR_TARGETS)
	$(AR) $(ARFLAGS) $(LIBC) $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	cp $(CRT0_OBJ) $(SCRT0_OBJ) $(CTOR_TARGETS) $(TOPDIR)lib/

$(CRT0_OBJ): $(CRT0_SRC) crtreloc.o
	$(CC) $(CFLAGS) -DL_$* -r -nostdlib $< crtreloc.o -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

crtreloc.o: crtreloc.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SCRT0_OBJ): $(CRT0_SRC) Scrtreloc.o
	$(CC) $(CFLAGS) -fPIE -DL_$* -r -nostdlib $< Scrtreloc.o -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

Scrtreloc.o: crtreloc.c
	$(CC) $(CFLAGS) -fPIE -c $< -o $@

$(CTOR_TARGETS): %.o : %.S
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(SOBJS): %.o : %.S
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

ifeq ($(strip $(UCLIBC_PROFILING)),y)
SAFECFLAGS := $(filter-out -g,$(CFLAGS))
gmon-start.S: ../common/gmon-start.c
	$(CC) $(SAFECFLAGS) -c $< -S -o $*.S
gcrt1.o: $(CRT0_DEPS)
endif

headers:


clean:
	rm -f *.[oa] *~ core
	rm -f bits/sysnum.h

