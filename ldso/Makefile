TOPDIR=../
include $(TOPDIR)Rules.mak

SUBDIRS = libdl
ALL_SUBDIRS = util libdl d-link



all:
ifeq ($(strip $(HAVE_SHARED)),true)
	@if [ -d $(TOPDIR)ldso/d-link/$(TARGET_ARCH) ] ; then \
		$(MAKE) -C d-link; \
	fi;
endif

shared:
ifeq ($(strip $(HAVE_SHARED)),true)
	@if [ -d $(TOPDIR)ldso/d-link/$(TARGET_ARCH) ] ; then \
		set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d ; done \
	fi;
endif

clean:
	set -e ; for d in $(ALL_SUBDIRS) ; do $(MAKE) -C $$d $@ ; done
	-find . -name '*~' | xargs rm -f
