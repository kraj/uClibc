# Copyright (C) 1996 Robert de Bath <robert@mayday.compulink.co.uk>
# This file is part of the Linux-8086 C library and is distributed
# under the GNU Library General Public License.

TOPDIR=../
include $(TOPDIR)Rules.make

LIBC=$(TOPDIR)libc.a

MSRC=alloc.c
MOBJ=malloc.o realloc.o free.o calloc.o malloc_dbg.o free_dbg.o calloc_dbg.o

all: $(LIBC)($(MOBJ))

$(LIBC): $(MOBJ)

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(AR) $(ARFLAGS) $(LIBC) $*.o

clean:
	rm -f *.o libc.a


