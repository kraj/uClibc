TOPDIR=../../
include $(TOPDIR)/ldso/Rules.mak

CFLAGS += -DLDSO_ADDR="0x62f00020" # needed by ldd.o
#CFLAGS += -I./ -I../../include/
#LDFLAGS += -nostdlib $(TOPDIR)lib/libc.a $(TOPDIR)lib/crt0.o $(GCCINCDIR)/../libgcc.a
LDFLAGS += $(TOPDIR)lib/libc.a $(TOPDIR)lib/crt0.o $(GCCINCDIR)/../libgcc.a

ALL	= ldconfig ldd readelf # lddstub

all:	$(ALL)

CSRC= readelf.c ldconfig.c ldd.c readsoname.c
COBJS=$(patsubst %.c,%.o, $(CSRC))
OBJS=$(COBJS)

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

readsoname.o: readsoname.c readsoname2.c

readelf: readelf.o
	$(CC) $(CFLAGS) $^ -o $@

ldconfig: ldconfig.o readsoname.o
	$(CC) $(CFLAGS) $^ -o $@

ldd:	ldd.o readsoname.o
	$(CC) $(CFLAGS) $^ -o $@

#ifeq ($(DEBUG),true)
#STUBFLAGS = -Wl,-dynamic-linker,../d-link/ld-linux.so
#endif

#lddstub: lddstub.o
#	$(CC) $(CFLAGS) $(LDFLAGS) $(STUBFLAGS) $^ -o $@

clean:
	rm -f $(ALL) *.o *~ core

