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

TARGETS=test_pwd test_pwd_glibc
TARGETS+=test_grp test_grp_glibc
TARGETS+=test_pwd_diff test_grp_diff

all: $(TARGETS)

test_pwd: test_pwd.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	-./$@ 2>&1 >test_pwd.out
	-@ echo " "

test_pwd_glibc: test_pwd.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(YCFLAGS) -c $< -o $@.o
	$(CC) $(YLDFLAGS) --static $@.o -o $@
	-./$@ 2>&1 >test_pwd_glibc.out
	-@ echo " "

test_grp: test_grp.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	-./$@ 2>&1 >test_grp.out
	-@ echo " "

test_grp_glibc: test_grp.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(YCFLAGS) -c $< -o $@.o
	$(CC) $(YLDFLAGS) --static $@.o -o $@
	-./$@ 2>&1 >test_grp_glibc.out
	-@ echo " "

test_pwd_diff: test_pwd_glibc test_pwd
	-@ echo "-------"
	-@ echo " "
	-@ echo "Diffing output: "
	-@ echo " "
	-diff -u test_pwd_glibc.out test_pwd.out
	-@ echo " "

test_grp_diff: test_grp_glibc test_grp
	-@ echo "-------"
	-@ echo " "
	-@ echo "Diffing output: "
	-@ echo " "
	-diff -u test_grp_glibc.out test_grp.out
	-@ echo " "

clean:
	rm -f *.[oa] *~ core $(TARGETS) *.out


