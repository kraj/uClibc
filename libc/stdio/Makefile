# Makefile for uClibc
#
# Copyright (C) 2000 by Lineo, inc.
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
#
# Derived in part from the Linux-8086 C library, the GNU C Library, and several
# other sundry sources.  Files within this library are copyright by their
# respective copyright holders.

TOPDIR=../
include $(TOPDIR)Rules.mak
LIBC=$(TOPDIR)libc.a

MSRC=stdio.c
MOBJ=_stdio_init.o \
     _alloc_stdio_buffer.o _free_stdio_buffer_of_file.o _free_stdio_stream.o \
     clearerr.o feof.o ferror.o fileno.o \
     setbuffer.o setvbuf.o setbuf.o setlinebuf.o \
     fclose.o _fopen.o fopen.o freopen.o fdopen.o fflush.o fsfopen.o \
     fseek.o rewind.o ftell.o fgetpos.o fsetpos.o \
     fputc.o fgetc.o fgets.o gets.o fputs.o puts.o ungetc.o \
     fread.o fwrite.o getchar.o putchar.o _uClibc_fwrite.o _uClibc_fread.o

MSRC2=printf.c
MOBJ2=printf.o sprintf.o fprintf.o vprintf.o vsprintf.o vfprintf.o snprintf.o \
     vsnprintf.o asprintf.o vfnprintf.o fnprintf.o vdprintf.o

MSRC3=scanf.c
MOBJ3=scanf.o sscanf.o fscanf.o vscanf.o vsscanf.o vfscanf.o

CSRC=popen.c perror.c remove.c getdelim.c getline.c tmpfile.c tmpnam.c tmpnam_r.c
COBJS=$(patsubst %.c,%.o, $(CSRC))
OBJS=$(MOBJ) $(MOBJ2) $(MOBJ3) $(COBJS)


all: $(OBJS) $(LIBC)

$(LIBC): ar-target

ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBC) $(OBJS)

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJ2): $(MSRC2)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJ3): $(MSRC3)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(OBJ): Makefile

clean:
	rm -f *.[oa] *~ core

