# Makefile for uClibc's math library
#
# Copyright (C) 2001 by Lineo, inc.
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

LIBM=../libm.a
TARGET_CC= $(TOPDIR)/extra/gcc-uClibc/$(TARGET_ARCH)-uclibc-gcc

CSRC=acoshl.c asinhl.c asinl.c atanhl.c atanl.c bdtrl.c btdtrl.c cbrtl.c \
	chdtrl.c coshl.c ellpel.c ellpkl.c elliel.c ellikl.c ellpjl.c \
	exp10l.c exp2l.c expl.c fdtrl.c gammal.c gdtrl.c igamil.c igaml.c \
	incbetl.c incbil.c isnanl.c j0l.c j1l.c jnl.c ldrand.c log10l.c log2l.c \
	logl.c nbdtrl.c ndtril.c ndtrl.c pdtrl.c powl.c powil.c sinhl.c sinl.c \
	sqrtl.c stdtrl.c tanhl.c tanl.c unityl.c ynl.c \
	floorl.c polevll.c mtherr.c #cmplxl.c clogl.c
COBJS=$(patsubst %.c,%.o, $(CSRC))


OBJS=$(COBJS)

all: $(OBJS) $(LIBM)

$(LIBM): ar-target

ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBM) $(OBJS)

$(COBJS): %.o : %.c
	$(TARGET_CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(OBJ): Makefile

clean:
	rm -f *.[oa] *~ core



#-----------------------------------------


#all: mtstl lparanoi lcalc fltestl nantst testvect monotl libml.a

mtstl: libml.a mtstl.o $(OBJS)
	$(CC) $(CFLAGS) -o mtstl mtstl.o libml.a $(LIBS)

mtstl.o: mtstl.c

lparanoi: libml.a lparanoi.o setprec.o ieee.o econst.o $(OBJS)
	$(CC) $(CFLAGS) -o lparanoi lparanoi.o setprec.o ieee.o econst.o libml.a $(LIBS)

lparanoi.o: lparanoi.c
	$(CC) $(CFLAGS) -Wno-implicit -c lparanoi.c

econst.o: econst.c ehead.h

lcalc: libml.a lcalc.o ieee.o econst.o $(OBJS)
	$(CC) $(CFLAGS) -o lcalc lcalc.o ieee.o econst.o libml.a $(LIBS)

lcalc.o: lcalc.c lcalc.h ehead.h

ieee.o: ieee.c ehead.h

# Use $(OBJS) in ar command for libml.a if possible; else *.o
libml.a: $(OBJS) mconf.h
	ar -rv libml.a $(OBJS)
	ranlib libml.a


fltestl: fltestl.c libml.a
	$(CC) $(CFLAGS) -o fltestl fltestl.c libml.a

fltestl.o: fltestl.c

flrtstl: flrtstl.c libml.a
	$(CC) $(CFLAGS) -o flrtstl flrtstl.c libml.a

flrtstl.o: flrtstl.c

nantst: nantst.c libml.a
	$(CC) $(CFLAGS) -o nantst nantst.c libml.a

nantst.o: nantst.c

testvect: testvect.o libml.a
	$(CC) $(CFLAGS) -o testvect testvect.o libml.a

testvect.o: testvect.c
	$(CC) -g -c -o testvect.o testvect.c

monotl: monotl.o libml.a
	$(CC) $(CFLAGS) -o monotl monotl.o libml.a

monotl.o: monotl.c
	$(CC) -g -c -o monotl.o monotl.c

# Run test programs
check: mtstl fltestl testvect monotl libml.a
	-mtstl
	-fltestl
	-testvect
	-monotl

