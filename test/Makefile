TOPDIR=../
include $(TOPDIR)Rules.make


CFLAGS = -Wall -Os -fomit-frame-pointer -fno-builtin -nostdinc -I$(TOPDIR)include -I/usr/include/linux
LDFLAGS = -nostdlib -s
EXTRA_LIBS=/home/andersen/CVS/uC-libc/libc.a

# Allow alternative stripping tools to be used...
ifndef $(STRIPTOOL)
	STRIPTOOL = strip
endif
STRIP    = $(STRIPTOOL) --remove-section=.note --remove-section=.comment $@

hello: hello.c Makefile
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ $(EXTRA_LIBS)
	$(STRIP)

clean:
	rm -f *.[oa] *~ core hello


