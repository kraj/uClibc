TOPDIR=../
include $(TOPDIR)Rules.mak

SUBDIRS = libdl
ALL_SUBDIRS = util libdl d-link

all:
	@if [ -d $(TOPDIR)ldso/d-link/$(TARGET_ARCH) ] ; then \
		$(MAKE) -C d-link; \
	fi;

shared:
	@if [ -d $(TOPDIR)ldso/d-link/$(TARGET_ARCH) ] ; then \
		set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d ; done \
	fi;

clean:
	set -e ; for d in $(ALL_SUBDIRS) ; do $(MAKE) -C $$d $@ ; done
	-find . -name '*~' | xargs rm -f
