# Makefile for uClibc NPTL pthread library (Linux files)
#
# Copyright (C) 2005 Steven J. Hill <sjhill@realitydiluted.com>
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
# Makefile for uClibc

TOPDIR=../../../../../../
include $(TOPDIR)Rules.mak
include ../../../../Rules.mak

CSRC-LIBP-ST = pt-raise.c pthread_attr_getaffinity.c		\
	       pthread_attr_setaffinity.c pthread_getaffinity.c	\
	       pthread_getcpuclockid.c pthread_kill.c		\
	       pthread_mutex_cond_lock.c pthread_setaffinity.c	\
	       pthread_yield.c sem_post.c sem_timedwait.c	\
	       sem_trywait.c sem_wait.c pt-fork.c

CSRC-LIBP-SH = $(CSRC-LIBP-ST) 

CSRC-LIBC-ST = libc_pthread_init.c libc_multiple_threads.c	\
	       register-atfork.c unregister-atfork.c

CSRC-LIBC-SH = $(CSRC-LIBC-ST)

ifeq ($(strip $(TARGET_ARCH)),alpha)
CSRC-LIBC-ST += libc-lowlevellock.c
CSRC-LIBP-ST += rtld-libc-lowlevellock.c
endif
ifeq ($(strip $(TARGET_ARCH)),mips)
CSRC-LIBC-ST += libc-lowlevellock.c
CSRC-LIBP-ST += rtld-libc-lowlevellock.c
endif
ifeq ($(strip $(TARGET_ARCH)),powerpc)
CSRC-LIBC-ST += libc-lowlevellock.c
CSRC-LIBP-ST += rtld-libc-lowlevellock.c
endif
ifeq ($(strip $(TARGET_ARCH)),sparc)
CSRC-LIBC-ST += libc-lowlevellock.c
CSRC-LIBP-ST += rtld-libc-lowlevellock.c
endif

COBJ-LIBP-ST = $(patsubst %.c, %.o, $(CSRC-LIBP-ST))
COBJ-LIBP-SH = $(patsubst %.c, %.os, $(CSRC-LIBP-SH))
COBJ-LIBC-ST = $(patsubst %.c, %.o, $(CSRC-LIBC-ST))
COBJ-LIBC-SH = $(patsubst %.c, %.os, $(CSRC-LIBC-SH))

ar-target all: $(COBJ-LIBP-ST) $(COBJ-LIBP-SH)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment	\
		$(COBJ-LIBP-ST) $(COBJ-LIBP-SH)
else
	$(STRIPTOOL) -x -R .note -R .comment			\
		$(COBJ-LIBP-ST) $(COBJ-LIBP-SH)
endif
	$(AR) $(ARFLAGS) $(PTDIR)../$(LIBPTHREAD_STATIC_ARCHIVE) $(COBJ-LIBP-ST)
	$(AR) $(ARFLAGS) $(PTDIR)../$(LIBPTHREAD_SHARED_ARCHIVE) $(COBJ-LIBP-SH)

$(COBJ-LIBP-ST): %.o : %.c
	$(CC) $(CFLAGS-LIBP) -c $< -o $@

$(COBJ-LIBP-SH): %.os : %.c
	$(CC) $(CFLAGS-LIBP) -DSHARED -c $< -o $@

$(COBJ-LIBC-ST): %.o : %.c
	$(CC) $(CFLAGS-LIBC) -c $< -o $@

$(COBJ-LIBC-SH): %.os : %.c
	$(CC) $(CFLAGS-LIBC) -DSHARED -c $< -o $@

objs-pthread-libc: $(COBJ-LIBC-ST) $(COBJ-LIBC-SH)
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(STRIPTOOL) -X --strip-debug -R .note -R .comment	\
		$(COBJ-LIBC-ST) $(COBJ-LIBC-SH)
else
	$(STRIPTOOL) -x -R .note -R .comment			\
		$(COBJ-LIBC-ST) $(COBJ-LIBC-SH)
endif
	@for objfile in $(COBJ-LIBC-ST); do \
		echo ../libpthread/nptl/sysdeps/unix/sysv/linux/$$objfile >> $(PTCOBJST); \
	done
	@for objfile in $(COBJ-LIBC-SH); do \
		echo ../libpthread/nptl/sysdeps/unix/sysv/linux/$$objfile >> $(PTCOBJSH); \
	done

rtld-libc-lowlevellock.c: libc-lowlevellock.c
	$(LN) -fs $< $@

clean:
	$(RM) *.o *.os *~ core rtld-libc-lowlevellock.c
