# Makefile for uClibc
#
# Copyright (C) 2000-2003 Erik Andersen <andersen@uclibc.org>
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

TESTDIR=../
include $(TESTDIR)/Rules.mak

all: run


dltest: dltest.c
	$(CC) $(CFLAGS) -DLIBNAME="\"./libtest.so\"" dltest.c -ldl -lpthread -o dltest

libtest.so: libtest.c
	$(CC) $(CFLAGS) -fPIC -shared -Wl,-soname,libtest.so libtest.c -o libtest.so

# Second time, directly link libtest2.so with libpthread
dltest2: dltest.c
	$(CC) $(CFLAGS) -DLIBNAME="\"./libtest2.so\"" dltest.c -ldl -lpthread -o dltest2

libtest2.so: libtest.c
	$(CC) $(CFLAGS) -fPIC -shared -Wl,-soname,libtest2.so libtest.c -o libtest2.so -lpthread

run: dltest libtest.so dltest2 libtest2.so
	./dltest2
	./dltest

clean:
	rm -f *.o dltest dltest2 libtest.so libtest2.so

