TOPDIR=../
include $(TOPDIR)Rules.mak
include Config.mk

SUBDIRS = util d-link # man
TARGETDIR=/usr/$(TARGET_ARCH)-linux-uclibc



all:
	set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d ; done

install: all
	mkdir -p $(TARGETDIR)/etc
	mkdir -p $(TARGETDIR)/lib
	cp -a ./d-link/ld-linux-uclibc.so.1 ./d-link/libdl/libdl.so.1 $(TARGETDIR)/lib 
	./util/ldconfig

clean:
	set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@ ; done
	-find . -name '*~' | xargs rm -f
