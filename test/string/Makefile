TOPDIR=../../
include $(TOPDIR)Rules.mak

# Check if 'ls -sh' works or not 
LSFLAGS = $(shell if ls -sh >/dev/null 2>&1; \
    then echo "-sh"; else echo "-s" ; fi)

#XCFLAGS = -Wall -Os -fomit-frame-pointer -fno-builtin -nostdinc \
#	-I$(TOPDIR)include -I/usr/include/linux
#XLDFLAGS = -nostdlib -s -gc-sections
STRIPTOOL=/bin/true


XCFLAGS = -Wall -g -fno-builtin -nostdinc \
	-I$(TOPDIR)include -I/usr/include/linux
XLDFLAGS = -nostdlib -gc-sections
EXTRA_LIBS=$(TOPDIR)libc.a -lgcc

YCFLAGS = -Wall -Os -fomit-frame-pointer
YLDFLAGS = -s --static 


TARGETS=string string_glibc
TARGETS+=testcopy testcopy_glibc
TARGETS+=strerror strsignal

all: $(TARGETS)

string: string.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-./$@
	-@ echo " "

string_glibc: string.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(YCFLAGS) -c $< -o $@.o
	$(CC) $(YLDFLAGS) --static $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-./$@
	-@ echo " "

testcopy: testcopy.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-./$@ > testcopy.out
	-@ echo " "

testcopy_glibc: testcopy.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(YCFLAGS) -c $< -o $@.o
	$(CC) $(YLDFLAGS) --static $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-./$@ > testcopy.gnu.out
	-@ echo " "

testcopy_diff: testcopy testcopy_glibc
	-@ echo "-------"
	-@ echo " "
	-@ echo "Diffing output: "
	-@ echo " "
	-diff -u testcopy.gnu.out testcopy.out
	-@ echo " "


strerror: ../../string/strerror.c
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -DCHECK_BUF -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-./$@
	-@ echo " "

strsignal: ../../string/strsignal.c
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -DCHECK_BUF -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-./$@
	-@ echo " "


clean:
	rm -f *.[oa] *~ core $(TARGETS) testcopy.gnu.out testcopy.out


