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

MSRC=string.c
MOBJ=strlen.o strcat.o strcpy.o strchr.o strcmp.o strncat.o strncpy.o \
	strncmp.o strrchr.o strdup.o memcpy.o memccpy.o memset.o \
	memmove.o memcmp.o memchr.o ffs.o

MSRC1=index.c
MOBJ1=index.o rindex.o

MSRC2=strsignal.c
MOBJ2=strsignal.o psignal.o

CSRC=strpbrk.c strsep.c strstr.c strtok.c strcspn.c config.c strspn.c \
	strcasecmp.c strncasecmp.c strerror.c bcopy.c bzero.c bcmp.c \
	sys_errlist.c
COBJS=$(patsubst %.c,%.o, $(CSRC))
OBJS=$(MOBJ) $(MOBJ1) $(MOBJ2) $(COBJS)

all: $(OBJS) $(LIBC)

$(LIBC): ar-target

ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBC) $(OBJS)

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJ1): $(MSRC1)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJ2): $(MSRC2)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

clean:
	rm -f *.[oa] *~ core

