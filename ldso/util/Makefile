TOPDIR=../../
include $(TOPDIR)Rules.mak

all: ldconfig ldd readelf

readsoname.o: readsoname.c readsoname2.c
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

ldconfig.o: ldconfig.c
	$(TARGET_CC) $(TARGET_CFLAGS) -DUCLIBC_ROOT_DIR=\"$(ROOT_DIR)\" -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

ldd.o: ldd.c 
	$(TARGET_CC) $(TARGET_CFLAGS)  -DUCLIBC_ROOT_DIR=\"$(ROOT_DIR)\" \
	    -DLDSO_ADDR="0x62f00020" -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

readelf: readelf.c
	$(TARGET_CC) $(TARGET_CFLAGS) readelf.c -o $@

ldconfig: ldconfig.o readsoname.o
	$(TARGET_CC) $(TARGET_CFLAGS) -static $^ -o $@

ldd:	ldd.o readsoname.o
	$(TARGET_CC) $(TARGET_CFLAGS) $^ -o $@

clean:
	rm -f ldconfig ldd readelf *.o *~ core

