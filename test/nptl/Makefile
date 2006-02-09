# Makefile for NPTL testsuite in uClibc
#
# Copyright (C) 2005, 2006 Steven J. Hill <sjhill@uclibc.org>
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

TARGET_ARCH := $(strip $(subst ",, $(strip $(TARGET_ARCH))))

INCLUDES := -I$(TOPDIR)include -I.				\
	    -I$(PTDIR)						\
            -I$(PTDIR)/sysdeps/unix/sysv/linux/$(TARGET_ARCH)	\
            -I$(PTDIR)/sysdeps/$(TARGET_ARCH)			\
            -I$(PTDIR)/sysdeps/unix/sysv/linux			\
            -I$(PTDIR)/sysdeps/pthread				\
            -I$(PTDIR)/sysdeps/pthread/bits			\
            -I$(PTDIR)/sysdeps/generic				\
	    -I$(TOPDIR)ldso/include				\
	    -I$(TOPDIR)ldso/ldso/$(TARGET_ARCH)

#
# CFLAGS for global use as well as individual tests
#
CFLAGS = -D_LIBC -D_GNU_SOURCE -D__USE_GNU $(INCLUDES)		\
	 -std=gnu99 -DNOT_IN_libc=1 -Os

ifeq ($(TARGET_ARCH),i386)
CFLAGS-tst-align.c = -malign-double -mpreferred-stack-boundary=4
endif
ifeq ($(TARGET_ARCH),i686)
CFLAGS-tst-align.c = -malign-double -mpreferred-stack-boundary=4 -msse
endif
CFLAGS-tst-initializers1.c = -W -Wall -Werror
CFLAGS-tst-tls3.c = tst-tls3mod.so
CFLAGS-tst-tls4.c = tst-tls4moda.so tst-tls4modb.so
CFLAGS-tst-tls5.c = tst-tls5mod.so

#
# LDFLAGS for global use as well as individual tests
#
LDFLAGS += -L$(TOPDIR)lib -lpthread
LDFLAGS-tst-cleanup4 = tst-cleanup4aux.o
LDFLAGS-tst-clock2 = -lrt
LDFLAGS-tst-cond11 = -lrt
LDFLAGS-tst-cond19 = -lrt
LDFLAGS-tst-rwlock14 = -lrt
LDFLAGS-tst-tls3 = -ldl -rdynamic
LDFLAGS-tst-tls4 = -ldl
LDFLAGS-tst-tls5 =  tst-tls5mod.so

#
# Flags to build and link DSO modules
#
SO_CFLAGS = $(CFLAGS) -fPIC -DPIC -DSHARED -shared -DNOT_IN_libc
SO_LDFLAGS = $(LDFLAGS) -shared -static-libgcc -L$(TOPDIR)lib
SO_LDFLAGS-tst-tls5mod.os=-Wl,-soname,tst-tls5mod.so

#
# DSO modules
#
OS_OBJS := $(patsubst %.c, %.os, $(wildcard tst-*mod*.c))
SO_OBJS := $(patsubst %.c, %.so, $(wildcard tst-*mod*.c))

#
# Test applications and support objects
#
OBJS := tst-cleanup4aux.o

TARGETS = tst-align tst-align2						\
	  tst-atfork1							\
	  tst-attr1 tst-attr2 tst-attr3					\
	  tst-barrier1 tst-barrier2 tst-barrier3 tst-barrier4		\
	  tst-basic1 tst-basic2 tst-basic4 tst-basic5 tst-basic6	\
	  tst-cancel1 tst-cancel8 tst-cancel10 tst-cancel12		\
	  tst-cancel13 tst-cancel14 tst-cancel15  tst-cancel16		\
	  tst-cancel19							\
	  tst-cleanup0 tst-cleanup1 tst-cleanup2 tst-cleanup3		\
	  tst-clock1 tst-clock2						\
	  tst-cond1 tst-cond2 tst-cond3 tst-cond4 tst-cond5 tst-cond6	\
	  tst-cond7 tst-cond8 tst-cond9 tst-cond10 tst-cond11		\
	  tst-cond12 tst-cond13 tst-cond14 tst-cond15 tst-cond16	\
	  tst-cond17 tst-cond18 tst-cond19 tst-cond20 tst-cond21	\
	  tst-detach1							\
	  tst-eintr1 tst-eintr2 tst-eintr3 tst-eintr4 tst-eintr5	\
	  tst-exec2 tst-exec3 tst-exec4					\
	  tst-flock1 tst-flock2						\
	  tst-fork1 tst-fork2 tst-fork3 tst-fork4			\
	  tst-initializers1						\
	  tst-join2 tst-join3						\
	  tst-key1 tst-key2 tst-key4					\
	  tst-kill1 tst-kill2 tst-kill3 tst-kill4 tst-kill5 tst-kill6	\
	  tst-mutex1 tst-mutex2 tst-mutex3 tst-mutex4 tst-mutex5	\
	  tst-mutex6 tst-mutex7 tst-mutex8 tst-mutex9 tst-mutex5a	\
	  tst-mutex7a							\
	  tst-once1 tst-once2 tst-once3 tst-once4			\
	  tst-popen1							\
	  tst-raise1							\
	  tst-rwlock1 tst-rwlock2 tst-rwlock3 tst-rwlock4 tst-rwlock5	\
	  tst-rwlock6 tst-rwlock7 tst-rwlock8 tst-rwlock9 tst-rwlock10	\
	  tst-rwlock11 tst-rwlock12 tst-rwlock13 tst-rwlock14		\
	  tst-sched1							\
	  tst-sem1 tst-sem2 tst-sem3 tst-sem4 tst-sem5 tst-sem6		\
	  tst-sem7 tst-sem8 tst-sem9					\
	  tst-signal1 tst-signal2 tst-signal3 tst-signal4 tst-signal5	\
	  tst-signal6							\
	  tst-spin1 tst-spin2 tst-spin3					\
	  tst-stack1 tst-stack2						\
	  tst-stdio1 tst-stdio2 tst-sysconf				\
	  tst-tls1 tst-tls2 tst-tls3 tst-tls4 tst-tls5			\
	  tst-tsd1 tst-tsd2 tst-tsd3 tst-tsd4				\
	  tst-umask1

#
# Arguments used when running tests
#
tst-cancel7-ARGS = --command tst-cancel7
tst-exec4-ARGS = tst-exec4


all: $(OBJS) $(SO_OBJS) $(patsubst %, %, $(TARGETS))

$(patsubst %, %, $(TARGETS)): $(patsubst %, %.o, $(TARGETS))
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling $@ vs uClibc: "
	-@ echo " "
	$(CC) $(LDFLAGS) $(LDFLAGS-$@) $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-@ echo " "

$(patsubst %, %.o, $(TARGETS)): %.o : %.c
	$(CC) $(CFLAGS) $(CFLAGS-$<) -c $< -o $@

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OS_OBJS): %.os : %.c
	$(CC) $(SO_CFLAGS) -c $< -o $@

$(SO_OBJS): %.so : %.os
	$(CC) $(SO_LDFLAGS) $(SO_LDFLAGS-$<) $< -o $@

clean:
	$(RM) *.{o,os,so} *~ core $(TARGETS)
