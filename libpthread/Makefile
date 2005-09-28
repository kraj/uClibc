# Makefile for uClibc's pthread library
#
# Copyright (C) 2002 Erik Andersen <andersen@uclibc.org>
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

ALL_SUBDIRS = linuxthreads linuxthreads_db

DIRS=linuxthreads
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	DIRS+=linuxthreads_db
endif

all: subdirs

headers:
ifeq ($(strip $(UCLIBC_HAS_THREADS_NATIVE)),y)
	$(LN) -sf $(TOPDIR)libpthread/nptl/sysdeps/pthread/pthread.h $(TOPDIR)include/
	$(LN) -sf $(TOPDIR)libpthread/nptl/semaphore.h $(TOPDIR)include/
	$(LN) -sf ../$(TOPDIR)libpthread/nptl/sysdeps/unix/sysv/linux/$(TARGET_ARCH)/bits/semaphore.h $(TOPDIR)include/bits/
	$(LN) -sf ../$(TOPDIR)libpthread/nptl/sysdeps/unix/sysv/linux/$(TARGET_ARCH)/bits/pthreadtypes.h $(TOPDIR)include/bits/
	$(LN) -sf ../$(TOPDIR)libpthread/nptl/sysdeps/pthread/bits/libc-lock.h $(TOPDIR)include/bits/
	$(LN) -sf ../$(TOPDIR)libpthread/nptl/sysdeps/pthread/bits/stdio-lock.h $(TOPDIR)include/bits/
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(LN) -sf $(TOPDIR)libpthread/nptl_db/thread_db.h $(TOPDIR)include/
endif
else
	$(LN) -sf $(TOPDIR)libpthread/linuxthreads/sysdeps/pthread/pthread.h $(TOPDIR)include/
	$(LN) -sf $(TOPDIR)libpthread/linuxthreads/semaphore.h $(TOPDIR)include/
	$(LN) -sf ../$(TOPDIR)libpthread/linuxthreads/sysdeps/pthread/bits/pthreadtypes.h $(TOPDIR)include/bits/
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(LN) -sf $(TOPDIR)libpthread/linuxthreads_db/thread_db.h $(TOPDIR)include/
endif
endif

tags:
	ctags -R

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(ALL_SUBDIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(ALL_SUBDIRS)) : dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

clean: subdirs_clean
	$(RM)	$(TOPDIR)include/pthread.h $(TOPDIR)include/semaphore.h \
		$(TOPDIR)include/thread_db.h \
		$(TOPDIR)include/bits/pthreadtypes.h $(TOPDIR)include/bits/semaphore.h \
		$(TOPDIR)include/bits/libc-lock.h $(TOPDIR)include/bits/stdio-lock

.PHONY: dummy
