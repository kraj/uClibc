# Makefile for TLS testsuite in uClibc
#
# Copyright (C) 2005 Steven J. Hill <sjhill@uclibc.org>
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

TOPDIR = ../../
PTDIR = $(TOPDIR)libpthread/nptl
include ../Rules.mak

INCLUDES := -I. -I$(TOPDIR)include				\
	    -I$(PTDIR)						\
	    -I$(PTDIR)/compat					\
            -I$(PTDIR)/sysdeps/unix/sysv/linux/$(TARGET_ARCH)	\
            -I$(PTDIR)/sysdeps/$(TARGET_ARCH)			\
            -I$(PTDIR)/sysdeps/unix/sysv/linux			\
            -I$(PTDIR)/sysdeps/pthread				\
            -I$(PTDIR)/sysdeps/pthread/bits			\
            -I$(PTDIR)/sysdeps/generic				\
            -include $(PTDIR)/compat/libc-symbols.h		\
	    -I$(TOPDIR)ldso/include

CFLAGS = -D_LIBC -D_GNU_SOURCE $(INCLUDES) -std=gnu99		\
	 -DNOT_IN_libc=1 -DIS_IN_libpthread=1 -Os

LDFLAGS +=

#
# Targets and defines to build DSO modules
#
SO_CFLAGS = $(CFLAGS) -fPIC -DSHARED -shared

SO_LDFLAGS = $(LDFLAGS) -shared -static-libgcc -L$(TOPDIR)lib
SO_LDFLAGS-tst-tlsmod3.os = tst-tlsmod2.so
SO_LDFLAGS-tst-tlsmod8.os = tst-tlsmod7.so
SO_LDFLAGS-tst-tlsmod10.os = tst-tlsmod9.so
SO_LDFLAGS-tst-tlsmod12.os = tst-tlsmod11.so
SO_LDFLAGS-tst-tlsmod13a.os = tst-tlsmod13.so

OS_OBJS = $(patsubst %.c, %.os, $(wildcard tst-tlsmod*.c))
SO_OBJS = $(patsubst %.c, %.so, $(wildcard tst-tlsmod*.c))

#
# Test applications
#
APPS =	tst-tls1 tst-tls2 tst-tls3 tst-tls4 tst-tls5		\
	tst-tls6 tst-tls7 tst-tls8 tst-tls9 tst-tls10		\
	tst-tls11 tst-tls12 tst-tls13 tst-tls14 tst-tls15
#	tst-tls1-static tst-tls2-static tst-tls9-static

all: $(APPS)

$(OS_OBJS): %.os : %.c
	$(CC) $(SO_CFLAGS) -c $< -o $@

$(SO_OBJS): %.so : %.os
	$(CC) $(SO_LDFLAGS) $(SO_LDFLAGS-$<) $< -o $@

tst-tls1: tst-tls1.c
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls2: tst-tls2.c
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls3: tst-tls3.c tst-tlsmod1.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) $@.o tst-tlsmod1.so -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls4: tst-tls4.c tst-tlsmod2.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) -ldl $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls5: tst-tls5.c tst-tlsmod2.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) -ldl $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls6: tst-tls6.c tst-tlsmod2.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) -ldl $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls7: tst-tls7.c tst-tlsmod2.so tst-tlsmod3.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) -ldl $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls8: tst-tls8.c tst-tlsmod3.so tst-tlsmod4.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) -ldl $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls9: tst-tls9.c tst-tlsmod5.so tst-tlsmod6.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) -ldl $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls10: tst-tls10.c tst-tlsmod7.so tst-tlsmod8.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) -Wl,-rpath-link=. tst-tlsmod8.so $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls11: tst-tls11.c tst-tlsmod9.so tst-tlsmod10.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) -Wl,-rpath-link=. tst-tlsmod10.so $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls12: tst-tls12.c tst-tlsmod11.so tst-tlsmod12.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) -Wl,-rpath-link=. tst-tlsmod12.so $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls13: tst-tls13.c tst-tlsmod13.so tst-tlsmod13a.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) -ldl -Wl,-rpath-link=. $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls14: tst-tls14.c tst-tlsmod14a.so tst-tlsmod14b.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) -ldl -Wl,-rpath-link=. tst-tlsmod14a.so $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls15: tst-tls15.c tst-tlsmod15a.so tst-tlsmod15b.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) -ldl -Wl,-rpath-link=. $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls1-static: tst-tls1-static.c
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) -static $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls2-static: tst-tls2-static.c
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) -static $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

tst-tls9-static: tst-tls9-static.c tst-tlsmod5.so tst-tlsmod6.so
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) -ldl -static $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

clean:
	$(RM) *.o *.os *.so *~ core $(APPS)
