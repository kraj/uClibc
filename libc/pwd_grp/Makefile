# Copyright (C) 1996 Nat Friedman <ndf@aleph1.mit.edu>
# This file is part of the Linux-8086 C library and is distributed
# under the GNU Library General Public License.

LIBC=../libc.a

CC=m68k-pic-coff-gcc
AR=m68k-pic-coff-ar
RANLIB=m68k-pic-coff-ranlib

CCFLAGS= -O2 -m68000 -msoft-float -I../include

CFLAGS=$(ARCH) -ansi $(CCFLAGS) $(DEFS)

PSRC=__getpwent.c pwent.c getpwnam.c getpwuid.c putpwent.c getpw.c fgetpwent.c
GSRC=__getgrent.c grent.c getgrnam.c getgrgid.c fgetgrent.c initgroups.c \
	config-grp.h
USRC=utent.c

POBJ=__getpwent.o pwent.o getpwnam.o getpwuid.o putpwent.o getpw.o fgetpwent.o
GOBJ=__getgrent.o grent.o getgrnam.o getgrgid.o fgetgrent.o initgroups.o 
UOBJ=utent.o

OBJ=$(POBJ) $(GOBJ) $(UOBJ)

all: $(LIBC)($(OBJ))
	@$(RM) $(OBJ)

$(LIBC)($(GOBJ)): config-grp.h

clean:
	rm -f *.o libc.a

