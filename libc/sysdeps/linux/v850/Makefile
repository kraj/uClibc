#
# libc/sysdeps/linux/v850/Makefile -- Makefile for v850 machine-dependent dir
#
#  Copyright (C) 2001  NEC Corporation
#  Copyright (C) 2001  Miles Bader <miles@gnu.org>
#  Copyright (C) 2000  Lineo, inc.
#
# This file is subject to the terms and conditions of the GNU Library
# General Public License.  See the file COPYING.LIB in the main
# directory of this archive for more details.
#

TOPDIR=../../../../
include $(TOPDIR)Rules.mak
ASFLAGS=$(CFLAGS)

CFLAGS+= -I../ -D__ASSEMBLER__ -DASM_GLOBAL_DIRECTIVE=.globl

TARGET_MACHINE_TYPE=$(shell $(CC) -dumpmachine)

CRT0=crt0.S
CRT0_OBJ=$(patsubst %.S,%.o, $(CRT0))

SSRC=setjmp.S longjmp.S vfork.S
SOBJS=$(patsubst %.S,%.o, $(SSRC))

CSRC=
COBJS=$(patsubst %.c,%.o, $(CSRC))

OBJS=$(SOBJS) $(MOBJ) $(COBJS)


all: $(OBJS) $(LIBC)

$(LIBC): ar-target 

ar-target: $(OBJS) $(CRT0_OBJ)
	$(AR) $(ARFLAGS) $(LIBC) $(OBJS)
	cp $(CRT0_OBJ) $(TOPDIR)libc/$(CRT0_OBJ)

$(CRT0_OBJ): %.o : %.S
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

clean:
	rm -f *.[oa] *~ core
