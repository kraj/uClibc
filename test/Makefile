TOPDIR=../
include $(TOPDIR)Rules.make

# Check if 'ls -sh' works or not 
LSFLAGS = $(shell if ls -sh >/dev/null 2>&1; \
    then echo "-sh"; else echo "-s" ; fi)

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

TARGETS=hello_source hello_glibc hello
TARGETS+=testmalloc_source testmalloc_glibc testmalloc
TARGETS+=teststrtol_source teststrtol_glibc teststrtol teststrtol_diff

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
	-ldd $@
	ls $(LSFLAGS) $@
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
	-ldd $@
	ls $(LSFLAGS) $@
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
	$(STRIP)
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
	$(STRIP)
	ldd $@
	ls -sh $@
	-./$@ | tee $@.out
	-@ echo " "

teststrtol_glibc: teststrtol.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(YCFLAGS) -c $< -o $@.o
	$(CC) $(YLDFLAGS) --static $@.o -o $@
	$(STRIP)
	ldd $@
	ls -sh $@
	-./$@ | tee $@.out
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


