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

# Allow alternative stripping tools to be used...
ifndef $(STRIPTOOL)
	STRIPTOOL = strip
endif
STRIP    = $(STRIPTOOL) --remove-section=.note --remove-section=.comment $@

TARGETS=string string_glibc
TARGETS+=testcopy testcopy_glibc

all: $(TARGETS)

string: string.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uCLibc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	-./$@
	-@ echo " "

string_glibc: string.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(YCFLAGS) -c $< -o $@.o
	$(CC) $(YLDFLAGS) --static $@.o -o $@
	-./$@
	-@ echo " "

testcopy: testcopy.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uCLibc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	-./$@ > testcopy.out
	-@ echo " "

testcopy_glibc: testcopy.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(YCFLAGS) -c $< -o $@.o
	$(CC) $(YLDFLAGS) --static $@.o -o $@
	-./$@ > testcopy.gnu.out
	-@ echo " "

testcopy_diff: testcopy testcopy_glibc
	-@ echo "-------"
	-@ echo " "
	-@ echo "Diffing output: "
	-@ echo " "
	-diff -u testcopy.gnu.out testcopy.out
	-@ echo " "


clean:
	rm -f *.[oa] *~ core $(TARGETS)


