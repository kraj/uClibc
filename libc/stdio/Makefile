# Makefile for uClibc
#
# Copyright (C) 2000 by Lineo, inc.
# Copyright (C) 2000,2001 Erik Andersen <andersen@uclibc.org>
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

TOPDIR=../../
include $(TOPDIR)Rules.mak

# Set to true to use the old vfprintf instead of the new.  The old is roughly
# C89 compliant, but doesn't deal with qualifiers on %n and doesn't deal with
# %h correctly or %hh at all on the interger conversions.  But on i386 it is
# over 1.5k smaller than the new code.  Of course, the new code fixes the
# above mentioned deficiencies and adds custom specifier support similar to
# glibc, as well as handling positional args.  This option is here temporarily
# until the configuration system gets rewritten.  Note also that the old
# vfprintf code will be rewritten at some point to bring it into at least C89
# standards compliance.

USE_OLD_VFPRINTF = false

# Note: The *64.o objects are empty when compiled without large file support.
#       To not build them at all, remove the appropriate line from the MOBJ
#       definition and uncomment the DOLFS test below.

# Note: Use the libpthreads of: flockfile.o ftrylockfile.o funlockfile.o
#       Also, maybe move __fsetlocking.o as well?

MSRC = stdio.c
MOBJ = fclose.o fflush.o fopen.o freopen.o perror.o remove.o \
	setbuf.o setvbuf.o fgetc.o fgets.o fputc.o fputs.o \
	getc.o getchar.o gets.o putc.o putchar.o puts.o \
	ungetc.o fread.o fwrite.o fgetpos.o fseek.o fsetpos.o ftell.o \
	rewind.o clearerr.o feof.o ferror.o \
	fileno.o fdopen.o getw.o putw.o setbuffer.o setlinebuf.o fcloseall.o \
	fopen64.o freopen64.o ftello64.o fseeko64.o fsetpos64.o fgetpos64.o \
	__fbufsize.o __freading.o __fwriting.o __freadable.o __fwritable.o \
	__flbf.o __fpurge.o __fpending.o _flushlbf.o \
	 fopencookie.o fmemopen.o open_memstream.o \
	__fsetlocking.o flockfile.o ftrylockfile.o funlockfile.o \
	_stdio_fopen.o _stdio_fread.o _stdio_fwrite.o _stdio_adjpos.o \
	_stdio_lseek.o _stdio_init.o \
	_stdio_fsfopen.o _stdio_fdout.o _uintmaxtostr.o

# ifeq ($(DOLFS),true)
# 	MOBJ += fopen64.o freopen64.o ftello64.o fseeko64.o fsetpos64.o fgetpos64.o
# endif

MSRC2= printf.c
MOBJ2=  vsnprintf.o vdprintf.o vasprintf.o vprintf.o vsprintf.o \
	fprintf.o  snprintf.o  dprintf.o  asprintf.o  printf.o  sprintf.o

ifneq ($(USE_OLD_VFPRINTF),true)
	MOBJ2 += _ppfs_init.o _ppfs_prepargs.o _ppfs_setargs.o \
		 _ppfs_parsespec.o _do_one_spec.o vfprintf.o \
		 _store_inttype.o _load_inttype.o \
		 register_printf_function.o parse_printf_format.o
endif



ifeq ($(HAS_FLOATING_POINT),true)
	MOBJ2 += _dtostr.o
endif

MSRC3=scanf.c
MOBJ3=scanf.o sscanf.o fscanf.o vscanf.o vsscanf.o vfscanf.o

CSRC=popen.c getdelim.c getline.c tmpfile.c tmpnam.c \
     tmpnam_r.c tempnam.c ctermid.c
ifeq ($(USE_OLD_VFPRINTF),true)
	CSRC += old_vfprintf.c
endif
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

