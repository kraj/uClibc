# Makefile for uClibc
#
# Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
#
# Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
#

top_builddir=../
-include $(top_builddir).config

.EXPORT_ALL_VARIABLES:


ALL_SUBDIRS = \
	args assert crypt ctype pwd_grp signal silly stdlib string unistd \
	#misc
DIRS := $(ALL_SUBDIRS)

ifeq ($(HAVE_SHARED),y)
	DIRS += dlopen
endif
ifeq ($(UCLIBC_HAS_THREADS),y)
	DIRS += pthread
endif
ALL_SUBDIRS += pthread dlopen


all: subdirs

tags:
	ctags -R

clean: subdirs_clean

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(ALL_SUBDIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	@echo "Making test in $(patsubst _dir_%,%,$@)"
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(ALL_SUBDIRS)) : dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

.PHONY: dummy
