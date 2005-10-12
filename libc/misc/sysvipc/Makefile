# Makefile for uClibc
#
# Copyright (C) 2000 by Lineo, inc.
# Copyright (C) 2000,2001 Erik Andersen <andersen@uclibc.org>
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

TOPDIR=../../../
include $(TOPDIR)Rules.mak

MSRC := sem.c
MOBJ := semget.o semctl.o semop.o

MSRC2 := shm.c
MOBJ2 := shmat.o shmctl.o shmdt.o shmget.o

MSRC3 := msgq.c
MOBJ3 := msgctl.o msgget.o msgrcv.o msgsnd.o

CSRC := ftok.c 
COBJ := $(patsubst %.c,%.o, $(CSRC))

OBJS := $(MOBJ) $(MOBJ2) $(MOBJ3) $(COBJ)

OBJ_LIST := ../../obj.misc.sysvipc

all: $(OBJ_LIST)

$(OBJ_LIST): $(OBJS)
	$(STRIPTOOL) -x -R .note -R .comment $^
	echo $(patsubst %, misc/sysvipc/%, $^) > $@

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o

$(MOBJ2): $(MSRC2)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o

$(MOBJ3): $(MSRC3)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o

$(COBJ): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o *~ core
