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

CSRC= acoshf.c airyf.c asinf.c asinhf.c atanf.c \
	atanhf.c bdtrf.c betaf.c cbrtf.c chbevlf.c chdtrf.c \
	clogf.c cmplxf.c constf.c coshf.c dawsnf.c ellief.c \
	ellikf.c ellpef.c ellpkf.c ellpjf.c expf.c exp2f.c \
	exp10f.c expnf.c facf.c fdtrf.c floorf.c fresnlf.c \
	gammaf.c gdtrf.c hypergf.c hyp2f1f.c igamf.c igamif.c \
	incbetf.c incbif.c i0f.c i1f.c ivf.c j0f.c j1f.c \
	jnf.c jvf.c k0f.c k1f.c knf.c logf.c log2f.c \
	log10f.c nbdtrf.c ndtrf.c ndtrif.c pdtrf.c polynf.c \
	powif.c powf.c psif.c rgammaf.c shichif.c sicif.c \
	sindgf.c sinf.c sinhf.c spencef.c sqrtf.c stdtrf.c \
	struvef.c tandgf.c tanf.c tanhf.c ynf.c zetaf.c \
	zetacf.c polevlf.c setprec.c mtherr.c
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

