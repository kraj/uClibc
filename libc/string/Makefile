# Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
# This file is part of the Linux-8086 C library and is distributed
# under the GNU Library General Public License.

TOPDIR=../
include $(TOPDIR)Rules.make
LIBC=$(TOPDIR)libc.a

MSRC=string.c
MOBJ=strlen.o strcat.o strcpy.o strcmp.o strncat.o strncpy.o strncmp.o \
	strchr.o strrchr.o strdup.o memcpy.o memccpy.o memchr.o memset.o \
	memcmp.o memmove.o movedata.o
CFILES=strpbrk.c strsep.c strstr.c strtok.c strcspn.c \
    strspn.c strcasecmp.c strncasecmp.c config.c
COBJS=$(patsubst %.c,%.o, $(CFILES))


all: $(COBJS) $(MOBJ) $(LIBC)

$(COBJS): $(CFILES)
	$(CC) $(CFLAGS) -c $*.c -o $@

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* -c $(MSRC) -o $@

$(LIBC): $(COBJS) $(MOBJ)
	$(AR) $(ARFLAGS) $(LIBC) $(COBJS) $(MOBJ)

clean:
	rm -f *.o


