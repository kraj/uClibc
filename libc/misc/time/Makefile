# Copyright (C) 1996 Robert de Bath <robert@mayday.compulink.co.uk>
# This file is part of the Linux-8086 C library and is distributed
# under the GNU Library General Public License.

LIBC=../libc.a

CC=m68k-pic-coff-gcc
AR=m68k-pic-coff-ar
RANLIB=m68k-pic-coff-ranlib

CCFLAGS= -O2 -m68000 -msoft-float -I../include

OBJ=localtime.o gmtime.o asctime.o ctime.o asc_conv.o tm_conv.o mktime.o \
	localtime_r.o gmtime_r.o asctime_r.o ctime_r.o

CFLAGS=$(ARCH) $(CCFLAGS) $(DEFS)

all: $(LIBC)
	@$(RM) $(OBJ)

$(LIBC): $(LIBC)($(OBJ))

clean:
	rm -f *.o libc.a
