# Makefile for uClibc
#
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

TESTDIR=../
include $(TESTDIR)/Rules.mak

all: run

test1: test1.c
	$(CC) $(CFLAGS) -o test1 test1.c -ldl

test2: test2.c
	$(CC) $(CFLAGS) -o test2 test2.c -ldl

test3: test3.c
	$(CC) $(CFLAGS) -o test3 test3.c -ldl ./libtest1.so ./libtest2.so

libtest1.o: libtest1.c
	$(CC) $(CFLAGS) -fPIC -c libtest1.c -o libtest1.o

libtest2.o: libtest2.c
	$(CC) $(CFLAGS) -fPIC -c libtest2.c -o libtest2.o

libtest1.so: libtest1.o
	$(CC) $(CFLAGS) -fPIC -shared -o libtest1.so -Wl,-soname,libtest1.so libtest1.o ./libtest2.so

libtest2.so: libtest2.o
	$(CC) $(CFLAGS) -fPIC -shared -o libtest2.so -Wl,-soname,libtest2.so libtest2.o

run: libtest2.so libtest1.so test1 test2 test3
	@echo "----------running test 1--------------"
	-LD_LIBRARY_PATH=`pwd`:. LD_DEBUG=all ./test1
	@echo "----------running test 2--------------"
	-LD_LIBRARY_PATH=`pwd`:. LD_DEBUG=all ./test2
	@echo "----------running test 3--------------"
	-LD_LIBRARY_PATH=`pwd`:. LD_DEBUG=all ./test3

clean:
	rm -f *.o libtest1.so* libtest2.so* test1 test2 test3
