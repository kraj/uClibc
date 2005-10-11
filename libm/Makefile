# Makefile for uClibc's math library
#
# Copyright (C) 2000-2002 Erik Andersen <andersen@uclibc.org>
#
# The routines included in this math library are derived from the
# math library for Apple's MacOS X/Darwin math library, which was
# itself swiped from FreeBSD.  The original copyright information
# is as follows:
#
#     Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
#
#     Developed at SunPro, a Sun Microsystems, Inc. business.
#     Permission to use, copy, modify, and distribute this
#     software is freely granted, provided that this notice
#     is preserved.
#
# It has been ported to work with uClibc and generally behave
# by Erik Andersen <andersen@codepoet.org>
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

TOPDIR=../
include $(TOPDIR)Rules.mak

CFLAGS+=$(SSP_ALL_CFLAGS)

CFLAGS+=-D_IEEE_LIBM -D_ISOC99_SOURCE -D_SVID_SOURCE

LIB_NAME := libm
AR_LIB_NAME := $(TOPDIR)lib/$(LIB_NAME).a
SO_LIB_NAME = $(TOPDIR)lib/$(LIB_NAME).so
SO_FULL_NAME = $(LIB_NAME)-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

FL_MSRC := float_wrappers.c

ifeq ($(strip $(DO_C99_MATH)),y)
CSRC :=  e_acos.c e_acosh.c e_asin.c e_atan2.c e_atanh.c e_cosh.c\
         e_exp.c e_fmod.c e_gamma.c e_gamma_r.c e_hypot.c e_j0.c\
         e_j1.c e_jn.c e_lgamma.c e_lgamma_r.c e_log.c e_log10.c\
         e_pow.c e_remainder.c e_rem_pio2.c e_scalb.c e_sinh.c\
         e_sqrt.c k_cos.c k_rem_pio2.c k_sin.c k_standard.c k_tan.c\
         s_asinh.c s_atan.c s_cbrt.c s_ceil.c s_copysign.c s_cos.c\
         s_erf.c s_expm1.c s_fabs.c s_finite.c s_floor.c s_frexp.c\
         s_ilogb.c s_ldexp.c s_lib_version.c s_log1p.c s_logb.c\
         s_matherr.c s_modf.c s_nextafter.c s_rint.c s_scalbn.c\
         s_signgam.c s_significand.c s_sin.c s_tan.c s_tanh.c\
         w_acos.c w_acosh.c w_asin.c w_atan2.c w_atanh.c w_cabs.c\
         w_cosh.c w_drem.c w_exp.c w_fmod.c w_gamma.c w_gamma_r.c\
         w_hypot.c w_j0.c w_j1.c w_jn.c w_lgamma.c w_lgamma_r.c\
         w_log.c w_log10.c w_pow.c w_remainder.c w_scalb.c w_sinh.c\
         w_sqrt.c fpmacros.c nan.c s_ceilf.c s_floorf.c
FL_MOBJ := acosf.o acoshf.o asinf.o asinhf.o atan2f.o atanf.o atanhf.o cbrtf.o \
	ceilf.o copysignf.o cosf.o coshf.o erfcf.o erff.o exp2f.o expf.o \
	expm1f.o fabsf.o fdimf.o floorf.o fmaf.o fmaxf.o fminf.o fmodf.o \
	frexpf.o hypotf.o ilogbf.o ldexpf.o lgammaf.o log10f.o log1pf.o \
	log2f.o logbf.o logf.o lrintf.o lroundf.o modff.o nearbyintf.o \
	nextafterf.o powf.o remainderf.o remquof.o rintf.o roundf.o \
	scalblnf.o scalbnf.o sinf.o sinhf.o sqrtf.o tanf.o tanhf.o \
	tgammaf.o truncf.o
else
# This list of math functions was taken from POSIX/IEEE 1003.1b-1993
CSRC :=  w_acos.c w_asin.c s_atan.c w_atan2.c s_ceil.c s_cos.c \
	 w_cosh.c w_exp.c s_fabs.c s_floor.c w_fmod.c s_frexp.c \
	 s_ldexp.c w_log.c w_log10.c s_modf.c w_pow.c s_sin.c \
	 w_sinh.c w_sqrt.c s_tan.c s_tanh.c \
	 s_expm1.c s_scalbn.c s_copysign.c e_acos.c e_asin.c e_atan2.c \
	 k_cos.c e_cosh.c e_exp.c e_fmod.c e_log.c e_log10.c e_pow.c \
	 k_sin.c e_sinh.c e_sqrt.c k_tan.c e_rem_pio2.c k_rem_pio2.c \
	 s_finite.c
# We'll add sqrtf to avoid problems with libstdc++
FL_MOBJ := sqrtf.o
endif

ifeq ($(strip $(HAS_FPU)),y)
ifeq ($(strip $(DO_C99_MATH)),y)
ARCH_CSRC := $(wildcard $(TARGET_ARCH)/*.c)
ARCH_OBJS := $(patsubst %.c,%.o, $(ARCH_CSRC))
endif
endif

COBJS := $(patsubst %.c,%.o, $(CSRC))
OBJS := $(COBJS) $(FL_MOBJ)

ifeq ($(strip $(HAVE_SHARED)),y)
all: $(SO_LIB_NAME)
else
all: $(AR_LIB_NAME)
endif

$(AR_LIB_NAME): $(OBJS) $(ARCH_OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $@
	$(STRIPTOOL) -x -R .note -R .comment $^
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(AR) $(ARFLAGS) $@ $(ARCH_OBJS)

$(SO_LIB_NAME): $(AR_LIB_NAME)
	$(RM) $(TOPDIR)lib/$(SO_FULL_NAME) $(SO_LIB_NAME).$(MAJOR_VERSION) $@
	$(LD) $(LDFLAGS) -soname=$(LIB_NAME).so.$(MAJOR_VERSION) \
		-o $(TOPDIR)lib/$(SO_FULL_NAME) --whole-archive $< \
		--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
		-L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC)
	$(LN) -sf $(SO_FULL_NAME) $(SO_LIB_NAME).$(MAJOR_VERSION)
	$(LN) -sf $(SO_FULL_NAME) $@

$(COBJS) $(ARCH_OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(FL_MOBJ): $(FL_MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o

tags:
	ctags -R

clean:
	$(RM) *.o */*.o *~ core
