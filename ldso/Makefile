TOPDIR=../
include Rules.mak

SUBDIRS = util d-link libdl # man

all: shared

shared:
	@if [ -d $(TOPDIR)ldso/d-link/$(TARGET_ARCH) ] ; then \
		set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d ; done \
	fi;

clean:
	set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@ ; done
	-find . -name '*~' | xargs rm -f
