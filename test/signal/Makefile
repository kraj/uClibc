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

TARGETS=signal signal_glibc

all: $(TARGETS)

signal: signal.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(XCFLAGS) -c $< -o $@.o
	$(CC) $(XLDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	-./$@
	-@ echo " "

signal_glibc: signal.c Makefile $(TOPDIR)libc.a
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(YCFLAGS) -c $< -o $@.o
	$(CC) $(YLDFLAGS) --static $@.o -o $@
	-./$@
	-@ echo " "

clean:
	rm -f *.[oa] *~ core $(TARGETS)


