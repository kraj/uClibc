TOPDIR=../
include $(TOPDIR)Rules.mak
include Config.mk

SUBDIRS = util d-link libdl # man
TARGETDIR=/usr/$(TARGET_ARCH)-linux-uclibc



all:
	set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d ; done

install: all
	mkdir -p $(TARGETDIR)/etc
	mkdir -p $(TARGETDIR)/lib
	cp -a ./d-link/$(DLINKER).$(LDSO_VMAJOR) \
		./libdl/$(LIBDL).$(LDSO_VMAJOR) $(TARGETDIR)/lib 
	./util/ldconfig

clean:
	set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@ ; done
	-find . -name '*~' | xargs rm -f
