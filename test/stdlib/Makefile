TOPDIR=../../
include $(TOPDIR)Rules.mak

# Check if 'ls -sh' works or not 
LSFLAGS = $(shell if ls -sh >/dev/null 2>&1; \
    then echo "-sh"; else echo "-s" ; fi)

XCFLAGS = -Wall -Os -fomit-frame-pointer -fno-builtin -nostdinc \
	-I$(TOPDIR)include -I/usr/include/linux
XLDFLAGS = -nostdlib -s -gc-sections
EXTRA_LIBS=$(TOPDIR)libc.a -lgcc

YCFLAGS = -Wall -Os -fomit-frame-pointer
YLDFLAGS = -s --static 


TARGETS=testmalloc testmalloc_glibc 
TARGETS=mallocbug mallocbug_glibc
TARGETS+=teststrtol teststrtol_glibc teststrtol_diff

all: $(TARGETS)

testmalloc_source:
	-@ echo "-------"
	-@ echo "testmalloc.c source: "
	-@ echo " "
	-@ cat testmalloc.c
	-@ echo " "

testmalloc: testmalloc.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-ldd $@
	ls $(LSFLAGS) $@
	-./$@
	-@ echo " "

testmalloc_glibc: testmalloc.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(YCFLAGS) -c $< -o $@.o
	$(CC) $(YLDFLAGS) --static $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-ldd $@
	ls $(LSFLAGS) $@
	-./$@
	-@ echo " "

mallocbug: mallocbug.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uC-Libc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-ldd $@
	ls $(LSFLAGS) $@
	-./$@
	-@ echo " "

mallocbug_glibc: mallocbug.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(YCFLAGS) -c $< -o $@.o
	$(CC) $(YLDFLAGS) --static $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-ldd $@
	ls $(LSFLAGS) $@
	-./$@
	-@ echo " "

teststrtol_source:
	-@ echo "-------"
	-@ echo "teststrtol.c source: "
	-@ echo " "
	-@ cat teststrtol.c
	-@ echo " "

teststrtol: teststrtol.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uC-Libc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-ldd $@
	ls -sh $@
	-./$@ > $@.out
	-@ echo " "

teststrtol_glibc: teststrtol.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(YCFLAGS) -c $< -o $@.o
	$(CC) $(YLDFLAGS) --static $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-ldd $@
	ls -sh $@
	-./$@ > $@.out
	-@ echo " "

teststrtol_diff: teststrtol_glibc teststrtol
	-@ echo "-------"
	-@ echo " "
	-@ echo "Diffing output: "
	-@ echo " "
	-diff -u teststrtol_glibc.out teststrtol.out
	-@ echo " "

clean:
	rm -f *.[oa] *~ core $(TARGETS)


