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

MSRC=aliases.c
MOBJ=abs.o remove.o creat.o bcopy.o bzero.o # raise.o bcmp.o index.o rindex.o 

MSRC2=atexit.c
MOBJ2=on_exit.o atexit.o __do_exit.o exit.o

CSRC=atoi.c atol.c ltoa.c ltostr.c ctype.c qsort.c bsearch.c rand.c lsearch.c \
	getopt.c glob.c fnmatch.c itoa.c strtod.c strtol.c crypt.c sleep.c \
	mkstemp.c  mktemp.c realpath.c getenv.c putenv.c popen.c system.c \
	getcwd.c setenv.c execl.c execv.c execlp.c execvp.c execvep.c
COBJS=$(patsubst %.c,%.o, $(CSRC))


all: $(MOBJ) $(MOBJ2) $(COBJS) $(LIBC)

$(LIBC): $(MOBJ) $(MOBJ2) $(COBJS)
	$(AR) $(ARFLAGS) $(LIBC) $(MOBJ) $(MOBJ2) $(COBJS)

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o

$(MOBJ2): $(MSRC2)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o

$(COBJS): $(CSRC)


clean:
	rm -f *.[oa] *~ core

