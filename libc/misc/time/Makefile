# Copyright (C) 1996 Robert de Bath <robert@mayday.compulink.co.uk>
# This file is part of the Linux-8086 C library and is distributed
# under the GNU Library General Public License.

TOPDIR=../
include $(TOPDIR)Rules.make

CFLAGS+=-I$(TOPDIR)include/linux

LIBC=../libc.a

OBJ=localtime.o gmtime.o asctime.o ctime.o asc_conv.o tm_conv.o mktime.o \
	localtime_r.o gmtime_r.o asctime_r.o ctime_r.o

all: $(LIBC)

$(LIBC): $(LIBC)($(OBJ))

clean:
	rm -f *.o libc.a
