# Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
# This file is part of the Linux-8086 C library and is distributed
# under the GNU Library General Public License.

TOPDIR=../../../
include $(TOPDIR)Rules.make

CFLAGS+= -D_GCC_LIMITS_H_

LIBC=$(TOPDIR)libc.a

include makefile.objs

all: $(LIBC)

$(LIBC): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

transfer:
	-@rm -f ../include/stdio.h
	cp -p stdio.h ../include/.

clean:
	rm -f *.o
