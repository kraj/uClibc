# Makefile for uCLibc
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
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA 02111-1307 USA
#
# Derived in part from the Linux-8086 C library, the GNU C Library, and several
# other sundry sources.  Files within this library are copyright by their
# respective copyright holders.

TOPDIR=../
include $(TOPDIR)Rules.make
LIBC=$(TOPDIR)libc.a

MSRC=string.c
MOBJ=strlen.o strcat.o strcpy.o strcmp.o strncat.o strncpy.o strncmp.o \
	strchr.o strrchr.o strdup.o memcpy.o memccpy.o memset.o \
	memmove.o memcmp.o memchr.o

MSRC1=index.c
MOBJ1=index.o rindex.o

CSRC=strpbrk.c strsep.c strstr.c strtok.c strcspn.c config.c \
	strspn.c strcasecmp.c strncasecmp.c strerror.c sys_siglist.c \
	bcopy.c bzero.c bcmp.c
COBJS=$(patsubst %.c,%.o, $(CSRC))
OBJS=$(MOBJ) $(MOBJ1) $(COBJS)

all: $(OBJS) $(LIBC)

$(LIBC): ar-target

ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBC) $(OBJS)

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o

$(MOBJ1): $(MSRC1)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o

$(OBJS): Makefile

clean:
	rm -f *.[oa] *~ core

