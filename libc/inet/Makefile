# Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
# Copyright (C) 1998-1999 D. Jeff Dionne <jeff@rt-control.com>
# Copyright (C) 1998      Kenneth Albanowski <kjahds@kjahds.com>
# Copyright (C) 1999      D. Jeff Dionne <jeff@rt-control.com>
# under the GNU Library General Public License.

LIBC=../libc.a

CC=m68k-pic-coff-gcc
AR=m68k-pic-coff-ar
RANLIB=m68k-pic-coff-ranlib

CCFLAGS= -O2 -m68000 -msoft-float -fno-builtin -I../include

ASRC=addr.c
AOBJ=inet_aton.o inet_addr.o inet_ntoa.o

RSRC=resolv.c
ROBJ= encodeh.o decodeh.o encoded.o decoded.o lengthd.o encodeq.o \
decodeq.o lengthq.o encodea.o decodea.o encodep.o decodep.o \
formquery.o dnslookup.o resolveaddress.o resolvemailbox.o \
opennameservers.o closenameservers.o resolvename.o gethostbyname.o\
gethostbyaddr.o

OBJ=$(AOBJ) $(ROBJ)

CFLAGS=$(ARCH) $(CCFLAGS) $(DEFS)

all: $(LIBC)
	@$(RM) $(OBJ)

$(LIBC): $(LIBC)($(OBJ))

$(LIBC)($(AOBJ)): $(ASRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(AR) $(ARFLAGS) $@ $*.o

$(LIBC)($(ROBJ)): $(RSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(AR) $(ARFLAGS) $@ $*.o

clean:
	rm -f *.o libc.a
