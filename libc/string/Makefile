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

MSRCW= wstring.c
MOBJW=  basename.o bcopy.o bzero.o dirname.o ffs.o memccpy.o memchr.o memcmp.o \
	memcpy.o memmove.o mempcpy.o memrchr.o 	memset.o rawmemchr.o stpcpy.o \
	stpncpy.o strcasecmp.o strcasestr.o strcat.o strchrnul.o strchr.o \
	strcmp.o strcpy.o strcspn.o strdup.o strlen.o strncasecmp.o strncat.o \
	strncmp.o strncpy.o strndup.o strnlen.o strpbrk.o strrchr.o strsep.o \
	strspn.o strstr.o strtok.o strtok_r.o \
	 __xpg_basename.o # strcoll.o strerror.o strxfrm.o 

MOBJW2= wcscasecmp.o wcscat.o wcschrnul.o wcschr.o wcscmp.o wcscpy.o wcscspn.o \
	wcsdup.o wcslen.o wcsncasecmp.o wcsncat.o wcsncmp.o wcsncpy.o \
	wcsnlen.o wcspbrk.o wcsrchr.o wcsspn.o wcsstr.o wcstok.o wmemchr.o \
	wmemcmp.o wmemcpy.o wmemmove.o wmempcpy.o wmemset.o

MSRC=string.c
# MOBJ=strlen.o strcat.o strcpy.o strchr.o strcmp.o strncat.o strncpy.o \
# 	strncmp.o strrchr.o strdup.o strndup.o memcpy.o memccpy.o memset.o \
# 	memmove.o memcmp.o memchr.o ffs.o strnlen.o strxfrm.o stpcpy.o \
# 	stpncpy.o memrchr.o mempcpy.o
MOBJ=strxfrm.o

# ifeq ($(HAS_LOCALE),true)
# 	MOBJ += strcoll.o
# endif

MSRC1=strsignal.c
MOBJ1=strsignal.o psignal.o

MOBJ2=
#MSRC2=strstr.c
#MOBJ2=strstr.o strcasestr.o

# CSRC=strpbrk.c strsep.c strtok.c strtok_r.c strcspn.c \
# 	strspn.c strcasecmp.c strncasecmp.c strerror.c bcopy.c bzero.c \
# 	bcmp.c sys_errlist.c dirname.c basename.c
CSRC=strerror.c sys_errlist.c
COBJS=$(patsubst %.c,%.o, $(CSRC))
OBJS=$(MOBJ) $(MOBJ1) $(MOBJ2) $(COBJS) $(MOBJW)

ifeq ($(HAS_WCHAR),true)
	OBJS += $(MOBJW2)
endif

all: $(OBJS) $(LIBC)

$(LIBC): ar-target

ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBC) $(OBJS)

$(MOBJW): $(MSRCW)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJW2): $(MSRCW)
	$(CC) $(CFLAGS) -DWANT_WIDE -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJ2): $(MSRC2)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJ1): $(MSRC1)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

clean:
	rm -f *.[oa] *~ core

