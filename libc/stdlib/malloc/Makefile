# Copyright (C) 1996 Robert de Bath <robert@mayday.compulink.co.uk>
# This file is part of the Linux-8086 C library and is distributed
# under the GNU Library General Public License.

LIBC=../libc.a

CC=m68k-pic-coff-gcc
AR=m68k-pic-coff-ar
RANLIB=m68k-pic-coff-ranlib

CCFLAGS= -O2 -m68000 -msoft-float -I../include


MSRC=alloc.c
MOBJ=malloc.o free.o calloc.o malloc_dbg.o free_dbg.o calloc_dbg.o

OBJ=$(MOBJ)

CFLAGS=$(ARCH) $(CCFLAGS) $(DEFS)

all: $(LIBC)
	@$(RM) $(OBJ)

$(LIBC): $(LIBC)($(OBJ))

$(LIBC)($(MOBJ)): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(AR) $(ARFLAGS) $@ $*.o

clean:
	rm -f *.o libc.a


