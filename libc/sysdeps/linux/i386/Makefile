# Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
# This file is part of the Linux-8086 C library and is distributed
# under the GNU Library General Public License.

TOPDIR=../../../
include $(TOPDIR)Rules.make

LIBC=$(TOPDIR)libc.a
CFLAGS+= -D_GCC_LIMITS_H_

MSRC=syscalls.S
include makefile.objs
SFILES=setjmp.S longjmp.S _start.S _exit.S #fork.o
SOBJS=$(patsubst %.S,%.o, $(SFILES))
CFILES=readdir.c #select.c
COBJS=$(patsubst %.c,%.o, $(CFILES))


all: $(SOBJS) $(COBJS) $(MOBJ) $(LIBC)

$(SOBJS): $(SFILES)
	$(CC) $(CFLAGS) -c $*.S -o $@

$(COBJS): $(CFILES)
	$(CC) $(CFLAGS) -c $*.c -o $@

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* -c $(MSRC) -o $@

$(LIBC): $(SOBJS) $(COBJS) $(MOBJ)
	$(AR) $(ARFLAGS) $(LIBC) $(SOBJS) $(COBJS) $(MOBJ)

clean:
	rm -f *.o

