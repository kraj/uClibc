TOPDIR=../../
include $(TOPDIR)Rules.mak

all: ldconfig ldd readelf

readsoname.o: readsoname.c readsoname2.c
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o
    
ldconfig.o: ldconfig.c
	$(TARGET_CC) $(TARGET_CFLAGS) -DUCLIBC_ROOT_DIR=\"$(DEVEL_PREFIX)\" \
		-c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

readelf: readelf.c
	$(TARGET_CC) $(TARGET_CFLAGS) -static readelf.c -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@

ldconfig: ldconfig.o readsoname.o
	$(TARGET_CC) $(TARGET_CFLAGS) -static $^ -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@

ldd: ldd.c
	$(TARGET_CC) $(TARGET_CFLAGS) -DUCLIBC_ROOT_DIR=\"$(DEVEL_PREFIX)\" \
		-DUCLIBC_BUILD_DIR=\"$(shell cd $(TOPDIR); pwd)\" \
		-static ldd.c -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@

clean:
	rm -f ldconfig ldd readelf *.o *~ core

