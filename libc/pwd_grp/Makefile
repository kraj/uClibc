# Copyright (C) 1996 Robert de Bath <robert@debath.thenet.co.uk>
# This file is part of the Linux-8086 C library and is distributed
# under the GNU Library General Public License.

TOPDIR=../
include $(TOPDIR)Rules.make

LIBC=$(TOPDIR)libc.a

OBJ=__getpwent.o pwent.o getpwnam.o getpwuid.o putpwent.o getpw.o fgetpwent.o \
	__getgrent.o grent.o getgrnam.o getgrgid.o fgetgrent.o initgroups.o \
	utent.o

all: $(LIBC)($(OBJ))

$(LIBC): $(OBJ)

clean:
	rm -f *.o libc.a
