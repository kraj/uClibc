TOPDIR=../../
include $(TOPDIR)Rules.mak

CFLAGS=-Wall -Os -I- -I. --uclibc-use-build-dir
CC = $(TOPDIR)extra/gcc-uClibc/$(NATIVE_ARCH)-uclibc-gcc

all: ldconfig ldd readelf

readsoname.o: readsoname.c readsoname2.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

ldconfig.o: ldconfig.c
	$(CC) $(CFLAGS) -DUCLIBC_ROOT_DIR=\"$(ROOT_DIR)\" -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

ldd.o: ldd.c 
	$(CC) $(CFLAGS)  -DUCLIBC_ROOT_DIR=\"$(ROOT_DIR)\" \
	    -DLDSO_ADDR="0x62f00020" -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

readelf: readelf.c
	$(CC) $(CFLAGS) readelf.c -o $@

ldconfig: ldconfig.o readsoname.o
	$(CC) $(CFLAGS) -static $^ -o $@

ldd:	ldd.o readsoname.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f ldconfig ldd readelf *.o *~ core

