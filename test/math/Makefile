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



# Unix makefile for ieetst, eparanoi.
# Set LARGEMEM 1 in qcalc.h for 32-bit memory addresses.
# Define computer type and/or endianness in mconf.h.
#
# Configure eparanoi.c for desired arithmetic test;
# also define appropriate version of setprec.o, or use a stub that
# does no FPU setup.  To test native arithmetic, eparanoi uses
# the system libraries only; compile simply by `cc eparanoi.c -lm'.
#

TESTDIR=../
include $(TESTDIR)/Rules.mak


#CC = gcc
#CFLAGS= -O
INCS= mconf.h ehead.h
OBJS = ieee.o econst.o eexp.o elog.o epow.o etanh.o etodec.o mtherr.o #setprec.o
TARGETS=ieetst eparanoi

all: $(TARGETS)

ieetst: ieetst.o $(OBJS) drand.o $(INCS)
	$(CC) -o ieetst ieetst.o $(OBJS) drand.o -lc -lm

eparanoi: eparanoi.o $(OBJS) $(INCS)
	$(CC) -o eparanoi  eparanoi.o $(OBJS) -lc -lm

#setprec.o: setprec.387
#	as -o setprec.o setprec.387

#setprec.o: setprec.688
#	as -o setprec.o setprec.688

ieee.o: ieee.c $(INCS)
	$(CC) $(CFLAGS) -c ieee.c

econst.o: econst.c $(INCS)
	$(CC) $(CFLAGS) -c econst.c

elog.o: elog.c $(INCS)
	$(CC) $(CFLAGS) -c elog.c

eexp.o: eexp.c $(INCS)
	$(CC) $(CFLAGS) -c eexp.c

etanh.o: etanh.c $(INCS)
	$(CC) $(CFLAGS) -c etanh.c

epow.o: epow.c $(INCS)
	$(CC) $(CFLAGS) -c epow.c

mtherr.o: mtherr.c $(INCS)
	$(CC) $(CFLAGS) -c mtherr.c

ieetst.o: ieetst.c $(INCS)
	$(CC) $(CFLAGS) -c ieetst.c

drand.o: drand.c $(INCS)
	$(CC) $(CFLAGS) -c drand.c

etodec.o: etodec.c $(INCS)
	$(CC) $(CFLAGS) -c etodec.c

eparanoi.o: eparanoi.c $(INCS)
	$(CC) $(CFLAGS) -c eparanoi.c

clean:
	rm -f *.[oa] *~ core $(TARGETS)


