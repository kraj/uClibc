# Rules.make for uClibc test apps.
#
# Copyright (C) 2001 by Lineo, inc.
#
#
#Note: This does not read the top level Rules.mak file
#

include $(TESTDIR)../Config
include $(TESTDIR)Config


NATIVE_ARCH = $(shell uname -m | sed -e 's/i.86/i386/' -e 's/sparc.*/sparc/' \
	-e 's/arm.*/arm/g' -e 's/m68k.*/m68k/' -e 's/ppc/powerpc/')

# If you are running a cross compiler, you may want to set this
# to something more interesting...
CC = ../$(TESTDIR)extra/gcc-uClibc/$(NATIVE_ARCH)-uclibc-gcc
HOST_CC = gcc
STRIPTOOL=strip
LDD = ../$(TESTDIR)ldso/util/ldd


# Check if 'ls -sh' works or not
LSFLAGS = -l

# turn all the warnings on
WARNINGS=-Wall

# use '-Os' optimization if available, else use -O2
OPTIMIZATION = $(shell if $(CC) -Os -S -o /dev/null -xc /dev/null >/dev/null 2>&1; \
    then echo "-Os"; else echo "-O2" ; fi)

ifeq ($(DODEBUG),true)
    CFLAGS +=$(WARNINGS) $(OPTIMIZATION) -g
    GLIBC_CFLAGS +=$(WARNINGS) $(OPTIMIZATION) -g
    LDFLAGS =-Wl,-warn-common 
    GLIBC_LDFLAGS =-Wl,-warn-common 
    STRIPTOOL =/bin/true -Since_we_are_debugging
else
    CFLAGS  +=-fomit-frame-pointer
    GLIBC_CFLAGS  +=-fomit-frame-pointer
    LDFLAGS  =-s -Wl,-warn-common
    GLIBC_LDFLAGS  =-s -Wl,-warn-common
    STRIP    = $(STRIPTOOL) --remove-section=.note --remove-section=.comment $(PROG)
endif

ifneq ($(DODYNAMIC),true)
    LDFLAGS +=--static
endif
ifeq ($(strip $(HAVE_SHARED)),true)
ifeq ($(strip $(BUILD_UCLIBC_LDSO)),true)
CFLAGS+=--uclibc-use-build-dir
LDFLAGS+=--uclibc-use-build-dir
endif
endif
