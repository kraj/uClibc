# Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
# This file is part of the Linux-8086 C library and is distributed
# under the GNU Library General Public License.

TOPDIR=../
include $(TOPDIR)Rules.make
LIBC=$(TOPDIR)libc.a


MSRC=aliases.c
MOBJ=abs.o remove.o creat.o bcopy.o bzero.o 
	# raise.o bcmp.o index.o rindex.o 

MSRC2=atexit.c
MOBJ2=on_exit.o atexit.o __do_exit.o exit.o

CFILES=atoi.c atol.c ltoa.c ltostr.c ctype.c qsort.c bsearch.c rand.c lsearch.c \
	getopt.c glob.c fnmatch.c itoa.c strtod.c strtol.c crypt.c sleep.c \
	mkstemp.c  mktemp.c realpath.c getenv.c putenv.c popen.c system.c \
	getcwd.c setenv.c execl.c execv.c execlp.c execvp.c execvep.c
COBJS=$(patsubst %.c,%.o, $(CFILES))



all: $(COBJS) $(MOBJ) $(MOBJ2) $(LIBC)

$(COBJS): $(CFILES)
	$(CC) $(CFLAGS) -c $*.c -o $@

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* -c $(MSRC) -o $@

$(MOBJ2): $(MSRC2)
	$(CC) $(CFLAGS) -DL_$* -c $(MSRC2) -o $@

$(LIBC): $(COBJS) $(MOBJ) $(MOBJ2)
	$(AR) $(ARFLAGS) $(LIBC) $(COBJS) $(MOBJ) $(MOBJ2)

clean:
	rm -f *.o


