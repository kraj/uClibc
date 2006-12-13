# uClibc NPTL tests
# Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.

TESTS := tst-align tst-align2 tst-atfork1 tst-attr1 tst-attr2 tst-attr3	\
	tst-barrier1 tst-barrier2 tst-barrier3 tst-barrier4 tst-basic1	\
	tst-basic2 tst-basic3 tst-basic4 tst-basic5 tst-basic6		\
	tst-cancel1 tst-cancel2 tst-cancel3 tst-cancel6 tst-cancel7	\
	tst-cancel8 tst-cancel9 tst-cancel10 tst-cancel11 tst-cancel12	\
	tst-cancel13 tst-cancel14 tst-cancel15 tst-cancel16		\
	tst-cancel19 tst-cancel20 tst-cancel21 tst-cancel22		\
	tst-cleanup0 tst-cleanup1 tst-cleanup2 tst-cleanup3		\
	tst-cleanup4 tst-clock1 tst-clock2 tst-cond1 tst-cond2		\
	tst-cond3 tst-cond4 tst-cond5 tst-cond6 tst-cond7 tst-cond8	\
	tst-cond9 tst-cond10 tst-cond11 tst-cond12 tst-cond13		\
	tst-cond14 tst-cond15 tst-cond16 tst-cond17 tst-cond18		\
	tst-cond19 tst-cond20 tst-cond21 tst-detach1 tst-eintr1		\
	tst-eintr2 tst-eintr3 tst-eintr4 tst-eintr5 tst-exec2 tst-exec3	\
	tst-exec4 tst-exit1 tst-exit2 tst-exit3 tst-flock1 tst-flock2	\
	tst-fork1 tst-fork2 tst-fork3 tst-fork4 tst-initializers1	\
	tst-join1 tst-join2 tst-join3 tst-join4 tst-join5 tst-key1	\
	tst-key2 tst-key3 tst-key4 tst-kill1 tst-kill2 tst-kill3	\
	tst-kill4 tst-kill5 tst-kill6 tst-mutex1 tst-mutex2 tst-mutex3	\
	tst-mutex4 tst-mutex5 tst-mutex6 tst-mutex7 tst-mutex8		\
	tst-mutex9 tst-mutex5a tst-mutex7a tst-once1 tst-once2		\
	tst-once3 tst-once4 tst-popen1 tst-raise1 tst-rwlock1		\
	tst-rwlock2 tst-rwlock3 tst-rwlock4 tst-rwlock5 tst-rwlock6	\
	tst-rwlock7 tst-rwlock8 tst-rwlock9 tst-rwlock10 tst-rwlock11	\
	tst-rwlock12 tst-rwlock13 tst-rwlock14 tst-sched1 tst-sem1	\
	tst-sem2 tst-sem3 tst-sem4 tst-sem5 tst-sem6 tst-sem7 tst-sem8	\
	tst-sem9 tst-signal1 tst-signal2 tst-signal3 tst-signal4	\
	tst-signal5 tst-signal6 tst-spin1 tst-spin2 tst-spin3		\
	tst-stack1 tst-stack2 tst-stdio1 tst-stdio2 tst-sysconf		\
	tst-tls1 tst-tls2 tst-tls3 tst-tls4 tst-tls5 tst-tsd1 tst-tsd2	\
	tst-tsd3 tst-tsd4 tst-tsd5 tst-umask1

#
# These are for the RT library and POSIX timers.
#
TESTS += tst-clock tst-clock_nanosleep tst-cpuclock1 tst-cpuclock2	\
	tst-cputimer1 tst-cputimer2 tst-cputimer3 tst-mqueue1		\
	tst-mqueue2 tst-mqueue3 tst-mqueue4 tst-mqueue5 tst-mqueue6	\
	tst-mqueue7 tst-mqueue8 tst-mqueue9 tst-timer2 tst-timer3	\
	tst-timer4 tst-timer5

include ../Test.mak

TARGET_ARCH := $(strip $(subst ",, $(strip $(TARGET_ARCH))))
PTDIR := $(top_builddir)libpthread/nptl

EXTRA_CFLAGS := -DNOT_IN_libc=1 -D_LIBC -D__USE_GNU -std=gnu99 -I.	\
	-I$(PTDIR) -I$(PTDIR)/sysdeps/unix/sysv/linux/$(TARGET_ARCH)	\
	-I$(PTDIR)/sysdeps/$(TARGET_ARCH)				\
	-I$(PTDIR)/sysdeps/unix/sysv/linux				\
	-I$(PTDIR)/sysdeps/pthread					\
	-I$(PTDIR)/sysdeps/pthread/bits					\
	-I$(PTDIR)/sysdeps/generic					\
	-I$(top_builddir)ldso/include					\
	-I$(top_builddir)ldso/ldso/$(TARGET_ARCH)			\
	-include $(top_builddir)include/libc-symbols.h

ifeq ($(TARGET_ARCH),i386)
CFLAGS_tst-align.o := -malign-double -mpreferred-stack-boundary=4
endif
ifeq ($(TARGET_ARCH),i686)
CFLAGS_tst-align.o := -malign-double -mpreferred-stack-boundary=4 -msse
endif
CFLAGS_tst-cleanup4aux.o := -W -Wall -sjh
CFLAGS_tst-initializers1.o := -W -Wall -Werror
CFLAGS_tst-tls3.o := tst-tls3mod.so
CFLAGS_tst-tls4.o := tst-tls4moda.so tst-tls4modb.so
CFLAGS_tst-tls5.o := tst-tls5mod.so
CFLAGS_tst-tls3mod.so := -fPIC -DPIC -DSHARED -shared -DNOT_IN_libc
CFLAGS_tst-tls4moda.so := -fPIC -DPIC -DSHARED -shared -DNOT_IN_libc
CFLAGS_tst-tls4modb.so := -fPIC -DPIC -DSHARED -shared -DNOT_IN_libc
CFLAGS_tst-tls5mod.so := -fPIC -DPIC -DSHARED -shared -DNOT_IN_libc
CFLAGS_tst-tls5moda.so := -fPIC -DPIC -DSHARED -shared -DNOT_IN_libc
CFLAGS_tst-tls5modb.so := -fPIC -DPIC -DSHARED -shared -DNOT_IN_libc
CFLAGS_tst-tls5modc.so := -fPIC -DPIC -DSHARED -shared -DNOT_IN_libc
CFLAGS_tst-tls5modd.so := -fPIC -DPIC -DSHARED -shared -DNOT_IN_libc
CFLAGS_tst-tls5mode.so := -fPIC -DPIC -DSHARED -shared -DNOT_IN_libc
CFLAGS_tst-tls5modf.so := -fPIC -DPIC -DSHARED -shared -DNOT_IN_libc

EXTRA_LDFLAGS := -L$(top_builddir)lib -lpthread

LDFLAGS_tst-cleanup4 := tst-cleanup4aux.o
LDFLAGS_tst-clock2 := -lrt
LDFLAGS_tst-cond11 := -lrt
LDFLAGS_tst-cond19 := -lrt
LDFLAGS_tst-rwlock14 := -lrt
LDFLAGS_tst-tls3 := -ldl -rdynamic
LDFLAGS_tst-tls4 := -ldl
LDFLAGS_tst-tls5 :=  tst-tls5mod.so
LDFLAGS_tst-clock := -lrt
LDFLAGS_tst-clock_nanosleep := -lrt
LDFLAGS_tst-cpuclock1 := -lrt
LDFLAGS_tst-cpuclock2 := -lrt
LDFLAGS_tst-cputimer1 := -lrt
LDFLAGS_tst-cputimer2 := -lrt
LDFLAGS_tst-cputimer3 := -lrt
LDFLAGS_tst-mqueue1 := -lrt
LDFLAGS_tst-mqueue2 := -lrt
LDFLAGS_tst-mqueue3 := -lrt
LDFLAGS_tst-mqueue4 := -lrt
LDFLAGS_tst-mqueue5 := -lrt
LDFLAGS_tst-mqueue6 := -lrt
LDFLAGS_tst-mqueue7 := -lrt
LDFLAGS_tst-mqueue8 := -lrt
LDFLAGS_tst-mqueue9 := -lrt
LDFLAGS_tst-timer2 := -lrt
LDFLAGS_tst-timer3 := -lrt
LDFLAGS_tst-timer4 := -lrt
LDFLAGS_tst-timer5 := -lrt
LDFLAGS_tst-tls3mod.so := -shared -static-libgcc
LDFLAGS_tst-tls4moda.so := -shared -static-libgcc
LDFLAGS_tst-tls4modb.so := -shared -static-libgcc
LDFLAGS_tst-tls5mod.so := -shared -static-libgcc -Wl,-soname,tst-tls5mod.so
LDFLAGS_tst-tls5moda.so := -shared -static-libgcc
LDFLAGS_tst-tls5modb.so := -shared -static-libgcc
LDFLAGS_tst-tls5modc.so := -shared -static-libgcc
LDFLAGS_tst-tls5modd.so := -shared -static-libgcc
LDFLAGS_tst-tls5mode.so := -shared -static-libgcc
LDFLAGS_tst-tls5modf.so := -shared -static-libgcc

#
# Special case
#
tst-cleanup4aux.o:
	$(Q)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c tst-cleanup4aux.c -o $@

tst-cleanup4: tst-cleanup4aux.o
tst-tls3: tst-tls3mod.so
tst-tls4: tst-tls4moda.so tst-tls4modb.so
tst-tls5: tst-tls5mod.so tst-tls5moda.so tst-tls5modb.so	\
	  tst-tls5modc.so tst-tls5modd.so tst-tls5mode.so tst-tls5modf.so

OPTS_tst-cancel7 = --command ./tst-cancel7
OPTS_tst-exec4 = ./tst-exec4

RET_tst-clock2 := 1
RET_tst-cputimer1 := 1
RET_tst-cputimer2 := 1
RET_tst-cputimer3 := 1

WRAPPER := env LD_LIBRARY_PATH="$$PWD:.:$(LD_LIBRARY_PATH)"
