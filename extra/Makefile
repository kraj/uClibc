# Makefile for uClibc
#
# Copyright (C) 2000 by Lineo, inc.
# Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
#
# Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
#

top_builddir=../
include $(top_builddir)Rules.mak

DIRS = 
EXTRA_DIRS_TO_CLEAN = config

all: subdirs

tags:
	ctags -R

clean: subdirs_clean

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(DIRS) $(EXTRA_DIRS_TO_CLEAN))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(DIRS) $(EXTRA_DIRS_TO_CLEAN)) : dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

.PHONY: dummy
