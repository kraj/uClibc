# Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
# This file is part of the Linux-8086 C library and is distributed
# under the GNU Library General Public License.

TOPDIR=../
include $(TOPDIR)Rules.make

LIBC=../libc.a

MSRC=aliases.c
MOBJ=abs.o remove.o creat.o bcopy.o bzero.o
# raise.o bcmp.o index.o rindex.o 


ESRC=atexit.c
EOBJ=on_exit.o atexit.o __do_exit.o exit.o

GOBJ=atoi.o atol.o ltoa.o ltostr.o \
     ctype.o qsort.o bsearch.o rand.o lsearch.o getopt.o \
     glob.o fnmatch.o itoa.o strtol.o crypt.o sleep.o mkstemp.o \
     mktemp.o

UOBJ=getenv.o putenv.o popen.o system.o getcwd.o setenv.o \
     execl.o execv.o execlp.o execvp.o execvep.o


OBJ=$(MOBJ) $(EOBJ) $(GOBJ) $(UOBJ)

## No ELKS strtod() until BCC does 16 bit FP...
#ifneq ($(LIB_CPU),i86)
#OBJ+=strtod.o
#endif

all: $(LIBC)

$(LIBC): $(LIBC)($(OBJ))

$(LIBC)($(MOBJ)): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(AR) $(ARFLAGS) $@ $*.o

$(LIBC)($(EOBJ)): $(ESRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(AR) $(ARFLAGS) $@ $*.o

clean:
	rm -f *.o libc.a

$(LIBC)(strtol.o): strtol.c
	$(CC) -c -ansi $(ARCH) $(CCFLAGS) $(DEFS) $*.c
	$(AR) $(ARFLAGS) $@ $*.o

$(LIBC)(strtod.o): strtod.c
	$(CC) -c -ansi $(ARCH) $(CCFLAGS) $(DEFS) $*.c
	$(AR) $(ARFLAGS) $@ $*.o

$(LIBC)(crypt.o): crypt.c
	$(CC) -c -ansi $(ARCH) $(CCFLAGS) $(DEFS) $*.c
	$(AR) $(ARFLAGS) $@ $*.o
