TOPDIR=../
include $(TOPDIR)Rules.make


XCFLAGS = -Wall -Os -fomit-frame-pointer -fno-builtin -nostdinc \
	-I$(TOPDIR)include -I/usr/include/linux
XLDFLAGS = -nostdlib -s -gc-sections
EXTRA_LIBS=$(TOPDIR)libc.a

YCFLAGS = -Wall -Os -fomit-frame-pointer
YLDFLAGS = -s --static 

# Allow alternative stripping tools to be used...
ifndef $(STRIPTOOL)
	STRIPTOOL = strip
endif
STRIP    = $(STRIPTOOL) --remove-section=.note --remove-section=.comment $@

TARGETS=hello_source hello_glibc hello testmalloc_source testmalloc_glibc testmalloc

all: $(TARGETS)

hello_source:
	-@ rm -f $(TARGETS)
	-@ echo "-------"
	-@ echo "hello.c source: "
	-@ echo " "
	-@ cat hello.c
	-@ echo " "

hello: hello.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uC-Libc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIP)
	ldd $@
	ls -sh $@
	-./$@
	-@ echo " "

hello_glibc: hello.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(YCFLAGS) -c $< -o $@.o
	$(CC) $(YLDFLAGS) --static $@.o -o $@
	$(STRIP)
	ldd $@
	ls -sh $@
	-./$@
	-@ echo " "

testmalloc_source:
	-@ echo "-------"
	-@ echo "testmalloc.c source: "
	-@ echo " "
	-@ cat testmalloc.c
	-@ echo " "

testmalloc: testmalloc.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uC-Libc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIP)
	ldd $@
	ls -sh $@
	-./$@
	-@ echo " "

testmalloc_glibc: testmalloc.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(YCFLAGS) -c $< -o $@.o
	$(CC) $(YLDFLAGS) --static $@.o -o $@
	$(STRIP)
	ldd $@
	ls -sh $@
	-./$@
	-@ echo " "

clean:
	rm -f *.[oa] *~ core $(TARGETS)


