# Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
# This file is part of the Linux-8086 C library and is distributed
# under the GNU Library General Public License.

TOPDIR=../../../
include $(TOPDIR)Rules.make

CFLAGS+= -D_GCC_LIMITS_H_

LIBC=$(TOPDIR)libc.a

.S.s:
	$(CC) $(CFLAGS) -D__ASSEMBLY__ -traditional -E -o $*.s $<
.S.o:
	$(CC) $(CFLAGS) -c -o $*.o $<

SSRC=syscalls.S
OBJ= setjmp.o longjmp.o readdir.o _start.o _exit.o fork.o #select.o 
include makefile.objs

all: $(LIBC)

$(LIBC): $(OBJ) $(LIBC)($(SOBJ))
	$(AR) $(ARFLAGS) $@ $(OBJ)

$(LIBC)($(SOBJ)): $(SSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(AR) $(ARFLAGS) $@ $*.o

transfer:
	-@rm -f ../include/stdio.h
	cp -p stdio.h ../include/.

clean:
	rm -f *.o
