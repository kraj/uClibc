# Makefile for uClibc's math library
# Copyright (C) 2001 by Lineo, inc.
#
# This math library is derived primarily from the Cephes Math Library,
# copyright by Stephen L. Moshier <moshier@world.std.com>
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

TOPDIR=../
include $(TOPDIR)Rules.mak

LIBM=libm.a
LIBM_SHARED=libm.so
LIBM_SHARED_FULLNAME=libm-$(MAJOR_VERSION).$(MINOR_VERSION).so
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
ALL_SUBDIRS = float double ldouble

all: $(LIBM)

$(LIBM): subdirs
	@if [ -f $(LIBM) ] ; then \
		install -d $(TOPDIR)lib; \
		rm -f $(TOPDIR)lib/$(LIBM); \
		install -m 644 $(LIBM) $(TOPDIR)lib; \
	fi;

tags:
	ctags -R
	
shared: all
	if [ -f $(LIBM) ] ; then \
		$(TARGET_CC) $(LDFLAGS) -shared -o $(LIBM_SHARED_FULLNAME) \
		    -Wl,-soname,$(LIBM_SHARED).$(MAJOR_VERSION) -Wl,--whole-archive \
		    $(LIBM) $(TOPDIR)lib/$(SHARED_FULLNAME); \
		install -d $(TOPDIR)lib; \
		rm -f $(TOPDIR)lib/$(LIBM_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBM_SHARED).$(MAJOR_VERSION); \
		install -m 644 $(LIBM_SHARED_FULLNAME) $(TOPDIR)lib; \
		(cd $(TOPDIR)lib; ln -sf $(LIBM_SHARED_FULLNAME) $(LIBM_SHARED)); \
		(cd $(TOPDIR)lib; ln -sf $(LIBM_SHARED_FULLNAME) $(LIBM_SHARED).$(MAJOR_VERSION)); \
	fi;

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(ALL_SUBDIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(ALL_SUBDIRS)) : dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

clean: subdirs_clean
	rm -f *.[oa] *~ core $(LIBM_SHARED)* $(LIBM_SHARED_FULLNAME)*

.PHONY: dummy


