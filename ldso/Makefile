TOPDIR=../
include $(TOPDIR)Rules.mak

ALL_SUBDIRS = d-link libdl util


all:
ifeq ($(strip $(BUILD_UCLIBC_LDSO)),true)
	$(MAKE) -C $(LIBRARY_CACHE) d-link;
endif

shared:
ifeq ($(strip $(BUILD_UCLIBC_LDSO)),true)
	$(MAKE) -C $(LIBRARY_CACHE) libdl;
endif
	$(MAKE) -C $(LIBRARY_CACHE) util;

clean:
	set -e ; for d in $(ALL_SUBDIRS) ; do $(MAKE) -C $$d $@ ; done
	-find . -name '*~' | xargs rm -f
