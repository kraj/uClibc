# Makefile for uClibc
#
# Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
#
# Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
#

top_builddir=../
-include $(top_builddir).config
include Rules.mak

ALL_SUBDIRS := \
	args assert crypt ctype pwd_grp signal silly stat stdlib string unistd \
	mmap misc
DIRS := $(ALL_SUBDIRS)

ifeq ($(HAVE_SHARED)$(UCLIBC_HAS_THREADS),yy)
ifeq ($(findstring -static,$(LDFLAGS)),)
	DIRS += dlopen
endif
endif
ifeq ($(UCLIBC_HAS_THREADS),y)
	DIRS += pthread
endif
ifeq ($(UCLIBC_HAS_FULL_RPC),y)
	DIRS += rpc
endif
ALL_SUBDIRS += dlopen pthread rpc
ALL_SUBDIRS := $(sort $(ALL_SUBDIRS))
DIRS := $(sort $(DIRS))


test check all: subdirs

tags:
	ctags -R

clean: subdirs_clean

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(ALL_SUBDIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(Q)$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(ALL_SUBDIRS)) : dummy
	$(Q)$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

.PHONY: all check clean dummy subdirs subdirs_clean test
