# Makefile for uClibc
#
# Copyright (C) 2000 by Lineo, inc.
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

TOPDIR=../../
include $(TOPDIR)Rules.mak

ifeq ($(strip $(INCLUDE_RPC)),true)
	DIRS=rpc
else
	DIRS=
endif
ALL_SUBDIRS = rpc

MSRC=addr.c
MOBJ=inet_aton.o inet_addr.o inet_ntoa.o

MSRC2=resolv.c
MOBJ2=encodeh.o decodeh.o encoded.o decoded.o lengthd.o encodeq.o \
	decodeq.o lengthq.o encodea.o decodea.o encodep.o decodep.o \
	formquery.o dnslookup.o resolveaddress.o resolvemailbox.o \
	opennameservers.o closenameservers.o resolvename.o gethostbyname.o\
	res_init.o res_query.o gethostbyaddr.o \
	get_hosts_byname.o get_hosts_byaddr.o read_etc_hosts.o \
	gethostbyname2.o getnameinfo.o

MSRC3=socketcalls.c
MOBJ3= accept.o bind.o connect.o getpeername.o getsockname.o getsockopt.o \
	listen.o recv.o recvfrom.o recvmsg.o send.o sendmsg.o sendto.o \
	setsockopt.o shutdown.o socket.o socketpair.o 

CSRC =getservice.c getproto.c hostid.c getnetent.c getnetbynm.c getnetbyad.c \
	herror.c inet_net.c ntop.c

COBJS=$(patsubst %.c,%.o, $(CSRC))

OBJS=$(MOBJ) $(MOBJ2) $(MOBJ3) $(COBJS)


all: $(OBJS) $(LIBC)

$(LIBC): ar-target subdirs

ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBC) $(OBJS)

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJ2): $(MSRC2)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJ3): $(MSRC3)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

clean: subdirs_clean
	rm -f *.[oa] *~ core

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(ALL_SUBDIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(ALL_SUBDIRS)) : dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

.PHONY: dummy

