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

TOPDIR=../
include $(TOPDIR)Rules.mak

LIBM=libm.a
LIBM_SHARED=libm.so
TARGET_CC= $(TOPDIR)extra/gcc-uClibc/$(TARGET_ARCH)-uclibc-gcc

DIRS=
ifeq ($(strip $(HAS_FLOATS)),true)
	DIRS+=float
endif
ifeq ($(strip $(HAS_DOUBLE)),true)
	DIRS+=double
endif
ifeq ($(strip $(HAS_LONG_DOUBLE)),true)
	DIRS+=ldouble
endif
ALL_SUBDIRS = $(shell find * -type d -prune -name [a-z]\*)

all: $(LIBM)

$(LIBM): subdirs

tags:
	ctags -R
	
shared: $(LIBM)
	$(TARGET_CC) $(LDFLAGS) -shared -o $(LIBM_SHARED).$(MAJOR_VERSION) \
	    -Wl,-soname,$(LIBM_SHARED).$(MAJOR_VERSION) -Wl,--whole-archive $(LIBM) $(TOPDIR)$(SHARED_FULLNAME)

install: all
	install -d $(INSTALL_DIR)/lib
	install -m 644 $(LIBM) $(INSTALL_DIR)/lib/
	@if [ -f $(LIBM_SHARED).$(MAJOR_VERSION) ] ; then \
	    install -m 644 $(LIBM_SHARED).$(MAJOR_VERSION) $(INSTALL_DIR)/lib/; \
	    (cd $(INSTALL_DIR)/lib/;ln -sf $(LIBM_SHARED).$(MAJOR_VERSION) $(LIBM_SHARED)); \
	fi;

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(ALL_SUBDIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(ALL_SUBDIRS)) : dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

clean: subdirs_clean
	rm -f *.[oa] *~ core $(LIBM_SHARED)* $(LIBM)

.PHONY: dummy


