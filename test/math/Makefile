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


