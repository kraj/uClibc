# Makefile for uClibc
#
# Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
#
# Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
#

top_srcdir=../
top_builddir=../
include $(top_builddir)Rules.mak

CFLAGS += $(SSP_ALL_CFLAGS) -B$(top_builddir)lib -I$(top_builddir)ldso/include

TARGETS = ldd ldconfig

ifeq ($(UCLIBC_HAS_LOCALE),y)
TARGET_ICONV = iconv
else
TARGET_ICONV =
endif

ifeq ($(LDSO_CACHE_SUPPORT),y)
HOST_LDSO_CACHE_FLAG = -D__LDSO_CACHE_SUPPORT__=1
else
HOST_LDSO_CACHE_FLAG =
endif

# NOTE: We build the utils AFTER we have a uClibc-targeted toolchain.

ifeq ($(HAVE_SHARED),y)
all: $(TARGETS) $(TARGET_ICONV)
else
all: $(TARGET_ICONV)
endif

headers:
	@$(LN) -fs $(top_builddir)include/elf.h
	@$(LN) -fs $(top_builddir)include/link.h

readelf.c ldconfig.c ldd.c:	headers

readelf: readelf.c
	$(CC) $(CFLAGS) $^ -o $@
	$(STRIPTOOL) -s -x -R .note -R .comment $@

ifeq ($(UCLIBC_STATIC_LDCONFIG),y)
LDCONFIG_CFLAGS := -static
else
LDCONFIG_CFLAGS := $(PIEFLAG) $(LDPIEFLAG)
endif
ldconfig: ldconfig.c chroot_realpath.c
	$(CC) $(CFLAGS) $(LDCONFIG_CFLAGS) \
		-DUCLIBC_RUNTIME_PREFIX=\"$(RUNTIME_PREFIX)\" \
		-DUCLIBC_LDSO=$(UCLIBC_LDSO) \
		$^ -o $@
	$(STRIPTOOL) -s -x -R .note -R .comment $@

LDD_CFLAGS := $(PIEFLAG) $(LDPIEFLAG)
ldd: ldd.c
	$(CC) $(CFLAGS) $(LDD_CFLAGS) \
		-DUCLIBC_RUNTIME_PREFIX=\"$(RUNTIME_PREFIX)\" \
		-DUCLIBC_LDSO=$(UCLIBC_LDSO) \
		$^ -o $@
	$(STRIPTOOL) -s -x -R .note -R .comment $@

ICONV_CFLAGS := $(PIEFLAG) $(LDPIEFLAG)
iconv: $(top_srcdir)libc/misc/wchar/wchar.c
	$(CC) $(CFLAGS) $(ICONV_CFLAGS) \
		-DL_iconv_main \
		$^ -o $@
	$(STRIPTOOL) -s -x -R .note -R .comment $@

ifeq ($(HAVE_SHARED),y)
hostutils: ldd.host ldconfig.host readelf.host
else
hostutils: readelf.host
endif

ldd.host: ldd.c
	$(HOSTCC) $(HOSTCFLAGS) -Wl,-s \
 		-DUCLIBC_RUNTIME_PREFIX=\"$(RUNTIME_PREFIX)\" $(HOST_LDSO_CACHE_FLAG) \
		-DUCLIBC_LDSO=$(UCLIBC_LDSO) -I. -I$(top_builddir)ldso/include \
		$^ -o $@

ldconfig.host: ldconfig.c chroot_realpath.c
	$(HOSTCC) $(HOSTCFLAGS) -Wl,-s \
 		-DUCLIBC_RUNTIME_PREFIX=\"$(RUNTIME_PREFIX)\" $(HOST_LDSO_CACHE_FLAG) \
		-DUCLIBC_LDSO=$(UCLIBC_LDSO) -I. -I$(top_builddir)ldso/include \
		$^ -o $@

readelf.host: readelf.c
	$(HOSTCC) $(HOSTCFLAGS) -Wl,-s $^ -o $@

clean:
	$(RM) $(TARGETS) *.o *~ core elf.h link.h readelf iconv *.host

install: all
ifeq ($(HAVE_SHARED),y)
	$(INSTALL) -d $(PREFIX)$(RUNTIME_PREFIX)sbin
	$(INSTALL) -d $(PREFIX)$(RUNTIME_PREFIX)usr/bin
	$(INSTALL) -m 755 ldd $(PREFIX)$(RUNTIME_PREFIX)usr/bin/ldd
	$(INSTALL) -m 755 ldconfig $(PREFIX)$(RUNTIME_PREFIX)sbin/ldconfig
	#$(INSTALL) -m 755 readelf $(PREFIX)$(RUNTIME_PREFIX)usr/bin/readelf
endif
ifeq ($(UCLIBC_HAS_LOCALE),y)
	$(INSTALL) -m 755 iconv $(PREFIX)$(RUNTIME_PREFIX)usr/bin/iconv
endif
