TOPDIR=../
include $(TOPDIR)Rules.make


XCFLAGS = -Wall -Os -fomit-frame-pointer -fno-builtin -nostdinc \
	-I$(TOPDIR)include -I/usr/include/linux
XLDFLAGS = -nostdlib -s -gc-sections
EXTRA_LIBS=/home/andersen/CVS/uC-libc/libc.a

YCFLAGS = -Wall -Os -fomit-frame-pointer
YLDFLAGS = -s --static 

# Allow alternative stripping tools to be used...
ifndef $(STRIPTOOL)
	STRIPTOOL = strip
endif
STRIP    = $(STRIPTOOL) --remove-section=.note --remove-section=.comment $@

TARGETS=hello_source hello_glibc hello

all: $(TARGETS)

hello_source:
	-@ echo "-------"
	-@ echo "hello.c source: "
	-@ echo " "
	-@ cat hello.c
	-@ echo " "

hello: hello.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo -n "Compiling vs uC-Libc: "
	-@ $(CC) $(XCFLAGS) -c $< -o $@.o
	-@ $(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	-@ $(STRIP)
	-@ echo "done."
	-@ ldd $@
	-@ ls -sh $@
	-@ echo " "

hello_glibc: hello.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo -n "Compiling vs GNU libc: "
	-@ $(CC) $(YCFLAGS) -c $< -o $@.o
	-@ $(CC) $(YLDFLAGS) --static $@.o -o $@
	-@ $(STRIP)
	-@ echo "done."
	-@ ldd $@
	-@ ls -sh $@
	-@ echo " "

clean:
	rm -f *.[oa] *~ core $(TARGETS)


