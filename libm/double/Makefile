# Makefile for uClibc's math library
# Copyright (C) 2001 by Lineo, inc.
#
# This math library is derived primarily from the Cephes Math Library,
# copyright by Stephen L. Moshier <moshier@world.std.com>
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

TOPDIR=../../
include $(TOPDIR)Rules.mak

LIBM=../libm.a
TARGET_CC= $(TOPDIR)extra/gcc-uClibc/$(TARGET_ARCH)-uclibc-gcc

CSRC=acosh.c airy.c asin.c asinh.c atan.c atanh.c bdtr.c beta.c \
	btdtr.c cbrt.c chbevl.c chdtr.c clog.c cmplx.c const.c \
	cosh.c dawsn.c ei.c ellie.c ellik.c ellpe.c ellpj.c ellpk.c \
	exp.c exp10.c exp2.c expn.c fabs.c fac.c fdtr.c \
	fresnl.c gamma.c gdtr.c hyp2f1.c hyperg.c i0.c i1.c igami.c incbet.c \
	incbi.c igam.c isnan.c iv.c j0.c j1.c jn.c jv.c k0.c k1.c kn.c kolmogorov.c \
	log.c log2.c log10.c lrand.c nbdtr.c ndtr.c ndtri.c pdtr.c planck.c \
	polevl.c polmisc.c polylog.c polyn.c pow.c powi.c psi.c rgamma.c round.c \
	shichi.c sici.c sin.c sindg.c sinh.c spence.c stdtr.c struve.c \
	tan.c tandg.c tanh.c unity.c yn.c zeta.c zetac.c \
	sqrt.c floor.c setprec.c mtherr.c noncephes.c

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

#all: libmd.a mtst dtestvec monot dcalc paranoia

time-it: time-it.o
	$(CC) -o time-it time-it.o

time-it.o: time-it.c
	$(CC) -O2 -c time-it.c

dcalc: dcalc.o libmd.a
	$(CC) -o dcalc dcalc.o libmd.a

mtst: mtst.o libmd.a
	$(CC) -v -o mtst mtst.o libmd.a

mtst.o: mtst.c
	$(CC) -O2 -Wall -c mtst.c

dtestvec: dtestvec.o libmd.a
	$(CC) -o dtestvec dtestvec.o libmd.a

dtestvec.o: dtestvec.c
	$(CC) -g -c dtestvec.c

monot: monot.o libmd.a
	$(CC) -o monot monot.o libmd.a

monot.o: monot.c
	$(CC) -g -c monot.c

paranoia: paranoia.o setprec.o libmd.a
	$(CC) -o paranoia paranoia.o setprec.o libmd.a

paranoia.o: paranoia.c
	$(CC) $(CFLAGS) -Wno-implicit -c paranoia.c

libmd.a: $(OBJS) $(INCS)
	$(AR) rv libmd.a $(OBJS)

#clean:
#	rm -f *.o
#	rm -f mtst
#	rm -f paranoia
#	rm -f dcalc
#	rm -f dtestvec
#	rm -f monot
#	rm -f libmd.a
#	rm -f time-it
#	rm -f dtestvec


