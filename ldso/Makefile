# Makefile for uClibc
#
# Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
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

TOPDIR=../
include $(TOPDIR)Rules.mak

DIRS = ldso libdl

ifeq ($(HAVE_SHARED),y)
all: subdirs
else
all:
endif

LN_HEADERS      := $(patsubst %, include/%, elf.h)
LN_ARCH_HEADERS := $(patsubst %, include/%, dl-startup.h dl-syscalls.h dl-sysdep.h dl-debug.h)
HEADERS         := $(LN_HEADERS) $(LN_ARCH_HEADERS) include/dl-progname.h

headers: $(HEADERS)

$(LN_HEADERS):
	$(LN) -sf $(TOPDIR)../$@ $@

$(LN_ARCH_HEADERS):
	$(LN) -sf ../ldso/$(TARGET_ARCH)/$(patsubst include/%,%,$@) $@

include/dl-progname.h:
	echo '#include "$(TARGET_ARCH)/elfinterp.c"' \
		> include/dl-progname.h

headers_clean:
	$(RM) $(HEADERS)

clean: subdirs_clean headers_clean

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(DIRS))

$(patsubst %, _dir_%, $(DIRS)): headers
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(DIRS)): dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

.PHONY: dummy
