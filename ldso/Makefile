TOPDIR=../
include Rules.mak

SUBDIRS = util libdl # d-link man

all:
	@if [ -d $(TOPDIR)ldso/d-link/$(TARGET_ARCH) ] ; then \
		$(MAKE) -C d-link; \
	fi;

shared:
	@if [ -d $(TOPDIR)ldso/d-link/$(TARGET_ARCH) ] ; then \
		set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d ; done \
	fi;

clean:
	set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@ ; done
	-find . -name '*~' | xargs rm -f
