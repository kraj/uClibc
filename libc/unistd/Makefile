# Makefile for uClibc
#
# Copyright (C) 2000 by Lineo, inc.
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

TOPDIR=../
include $(TOPDIR)Rules.mak
LIBC=$(TOPDIR)libc.a
DIRS:=

CSRC=execl.c execlp.c execv.c execvep.c execvp.c getcwd.c getopt.c \
	sleep.c getpass.c sysconf_src.c getopt_vars.c

# TESTING -- comment this out if it breaks for you
ifeq ($(TARGET_ARCH), $(NATIVE_ARCH))
	SYSCONF = sysconf_native
else
	SYSCONF = sysconf_$(TARGET_ARCH).c
endif

COBJS=$(patsubst %.c,%.o, $(CSRC))

MSRC=gnu_getopt.c
MOBJ=_gnu_getopt_internal.o gnu_getopt_long.o gnu_getopt_long_only.o

# WARNING! MOBJ _must_ come after COBJS for link to pick getopt 
# over gnu_getopt when appropriate.
OBJS=$(COBJS) $(MOBJ)

all: $(SYSCONF) $(OBJS) $(LIBC)

$(LIBC): ar-target subdirs

ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBC) $(OBJS)

# We are cross-compiling so use the generic sysconf.c.
sysconf_$(TARGET_ARCH).c: sysconf.c
	@echo warning: sysconf_$(NATIVE_ARCH).c is older then sysconf.c so using generic sysconf.c
	@echo To build sysconf_$(NATIVE_ARCH).c run gen_sysconf \> sysconf_$(NATIVE_ARCH).c on
	@echo your target platform, place in the unistd directory, and rebuild
	cp -f sysconf.c sysconf_$(TARGET_ARCH).c

# We are compiling for the native platform, so build an optimized sysconf.c.
getpagesize_tester.o:
	$(CC) $(CFLAGS) -D_UCLIBC_GENERATE_SYSCONF_ARCH \
	    -c ../sysdeps/linux/common/getpagesize.c -o getpagesize_tester.o

sysconf_tester.o:
	$(CC) $(CFLAGS) -D_UCLIBC_GENERATE_SYSCONF_ARCH -c sysconf.c \
	    -o sysconf_tester.o

gen_sysconf_tester.o: sysconf_tester.o getpagesize_tester.o
	@ld -r -o gen_sysconf_tester.o sysconf_tester.o getpagesize_tester.o
	@if nm -s gen_sysconf_tester.o | grep -v "U errno" | grep " U " ;\
	then \
		echo warning: missing symbols in gen_sysconf_tester.o so using generic sysconf.c ;\
		cp -f sysconf.c sysconf_src.c ;\
	else \
		if ../extra/gcc-uClibc/gcc-uClibc-$(NATIVE_ARCH) -static -D_UCLIBC_GENERATE_SYSCONF_MAIN sysconf.c sysconf_tester.o -o gen_sysconf && \
			./gen_sysconf > sysconf_$(NATIVE_ARCH).c ;\
		then \
			echo successfully built sysconf_$(NATIVE_ARCH).c ;\
		else \
			echo warning: build of gen_sysconf failed so using generic sysconf.c ;\
			cp -f sysconf.c sysconf_$(NATIVE_ARCH).c ;\
		fi ;\
	fi

sysconf_native: gen_sysconf_tester.o

sysconf_src.c: sysconf_$(TARGET_ARCH).c
	cp -f sysconf_$(TARGET_ARCH).c sysconf_src.c


$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(OBJ): Makefile

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(DIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

.PHONY: dummy sysconf_native
dummy:

clean:
	rm -f *.[oa] *~ core gen_sysconf sysconf_*.c

