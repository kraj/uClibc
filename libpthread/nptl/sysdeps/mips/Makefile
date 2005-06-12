# Makefile for uClibc's NPTL pthread library
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

TOPDIR=../../../../
include $(TOPDIR)Rules.mak

#
# FIX ME: We should probably be including 'nptl-sysdep.S' which needs
#         the new '__syscall_error' function for per-thread 'errno'. Maybe
#         this work and maybe it will not.
#
SOBJS = $(patsubst %.S,%.o, $(filter-out nptl-sysdep.S, $(wildcard *.S)))

OBJS = $(SOBJS)

all: $(OBJS)

$(SOBJS): %.o : %.S
	echo $(PTDIR)
	$(CC) $(ASFLAGS) -c $< -o $@

clean:
	$(RM) *.[oa] *~ core
