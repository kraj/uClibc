# Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
# This file is part of the Linux-8086 C library and is distributed
# under the GNU Library General Public License.

TOPDIR=../
include $(TOPDIR)Rules.make

LIBC=../libc.a

ASRC=stdio.c
AOBJ=_stdio_init.o fputc.o fgetc.o fflush.o fgets.o gets.o fputs.o	\
     puts.o fread.o fwrite.o fopen.o fclose.o fseek.o rewind.o ftell.o	\
     setbuffer.o setvbuf.o ungetc.o

PSRC=printf.c
POBJ=printf.o sprintf.o fprintf.o vprintf.o vsprintf.o vfprintf.o

SSRC=scanf.c
SOBJ=scanf.o sscanf.o fscanf.o vscanf.o vsscanf.o vfscanf.o

OBJ= $(AOBJ) $(POBJ) $(SOBJ) dputs.o

all: $(LIBC)

$(LIBC): $(LIBC)($(OBJ))

$(LIBC)($(AOBJ)): $(ASRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(AR) $(ARFLAGS) $@ $*.o

$(LIBC)($(POBJ)): $(PSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(AR) $(ARFLAGS) $@ $*.o

$(LIBC)($(SOBJ)): $(SSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(AR) $(ARFLAGS) $@ $*.o

#transfer:
#	-@rm -f ../include/stdio.h
#	cp -p stdio.h ../include/.
#
#$(LIBC)($(OBJ)): stdio.h

clean:
	rm -f *.o libc.a


