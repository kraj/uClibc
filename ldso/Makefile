TOPDIR=../
include Rules.mak

SUBDIRS = util d-link libdl # man


all:
	@if [ -d $(TOPDIR)ld.so-1/d-link/$(TARGET_ARCH) ] ; then \
		set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d ; done \
	fi;
	@if [ -d $(TOPDIR)ld.so-1/d-link/$(TARGET_ARCH) ] ; then \
		install -d $(TOPDIR)lib ; \
		install -m 755 $(TOPDIR)ld.so-1/d-link/$(UCLIBC_LDSO) $(TOPDIR)lib/ld-linux-uclibc.so ; \
		install -m 644 $(TOPDIR)ld.so-1/libdl/$(LIBDL).$(MAJOR_VERSION) $(TOPDIR)lib  ; \
		(cd $(TOPDIR)lib;ln -sf ld-linux-uclibc.so $(UCLIBC_LDSO)); \
		(cd $(TOPDIR)lib;ln -sf $(LIBDL).$(MAJOR_VERSION) $(LIBDL))  ; \
	fi;

ldso:
	@if [ -d $(TOPDIR)ld.so-1/d-link/$(TARGET_ARCH) ] ; then \
		$(MAKE) -C d-link; \
	fi;

clean:
	set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@ ; done
	-find . -name '*~' | xargs rm -f
