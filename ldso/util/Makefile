TOPDIR=../../
include $(TOPDIR)Rules.mak

all: ldconfig ldd readelf

readsoname.o: readsoname.c readsoname2.c
	$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o
    
ldconfig.o: ldconfig.c
	$(TARGET_CC) $(TARGET_CFLAGS) -DUCLIBC_TARGET_PREFIX=\"$(TARGET_PREFIX)\" \
		-c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

readelf: readelf.c
	$(TARGET_CC) $(TARGET_CFLAGS) -static readelf.c -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@

ifeq ($(strip $(LIBRARY_CACHE)),)
ldconfig: 
	echo "LIBRARY_CACHE disabled -- not building ldconfig"
else
ldconfig: ldconfig.o readsoname.o
	$(TARGET_CC) $(TARGET_CFLAGS) -static $^ -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
endif

ldd: ldd.c
	$(TARGET_CC) $(TARGET_CFLAGS) -DUCLIBC_TARGET_PREFIX=\"$(TARGET_PREFIX)\" \
		-DUCLIBC_DEVEL_PREFIX=\"$(DEVEL_PREFIX)\" \
		-DUCLIBC_BUILD_DIR=\"$(shell cd $(TOPDIR); pwd)\" \
		-DUCLIBC_LDSO=\"$(UCLIBC_LDSO)\" \
		-static ldd.c -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@

clean:
	rm -f ldconfig ldd readelf *.o *~ core

