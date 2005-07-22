# Makefile for uClibc's NPTL pthread debug library
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

TOPDIR=../../
include $(TOPDIR)Rules.mak

LIBTHREAD_DB=../libthread_db.a

CFLAGS := $(CFLAGS:-O0=-O2) -DNOT_IN_libc=1 -DIS_IN_libthread_db=1	  \
	  -std=gnu99 -DLIBPTHREAD_SO="\"libpthread.so.$(MAJOR_VERSION)\""

CFLAGS += -I$(PTDIR)compat \
	  -I$(PTDIR)sysdeps/unix/sysv/linux/$(TARGET_ARCH) \
	  -I$(PTDIR)sysdeps/$(TARGET_ARCH) \
          -I$(PTDIR)sysdeps/unix/sysv/linux \
	  -I$(PTDIR)sysdeps/pthread \
	  -I$(PTDIR)sysdeps/generic

OBJS=$(patsubst %.c, %.o, $(filter-out db_info.c, $(wildcard *.c)))

all: $(LIBTHREAD_DB)

$(LIBTHREAD_DB) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBTHREAD_DB) $(OBJS)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

clean:
	$(RM) *.[oa] *~ core
