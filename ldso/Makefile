TOPDIR=../
include Rules.mak

SUBDIRS = util d-link libdl # man


all:
	@if [ -d $(TOPDIR)ld.so-1/d-link/$(TARGET_ARCH) ] ; then \
		set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d ; done \
	fi;

install: all
	if [ -d $(TOPDIR)ld.so-1/d-link/$(TARGET_ARCH) ] ; then \
		install -d $(INSTALL_DIR)/lib $(INSTALL_DIR)/etc ; \
		install -m 755 $(TOPDIR)ld.so-1/d-link/$(DLINKER).$(LDSO_VMAJOR) $(INSTALL_DIR)/lib/ ; \
		install -m 644 $(TOPDIR)ld.so-1/libdl/$(LIBDL).$(LDSO_VMAJOR) $(INSTALL_DIR)/lib/  ; \
		(cd $(INSTALL_DIR)/lib/;ln -sf $(DLINKER).$(LDSO_VMAJOR) $(DLINKER))  ; \
		(cd $(INSTALL_DIR)/lib/;ln -sf $(LIBDL).$(LDSO_VMAJOR) $(LIBDL))  ; \
	fi;

ldso:
	@if [ -d $(TOPDIR)ld.so-1/d-link/$(TARGET_ARCH) ] ; then \
		$(MAKE) -C d-link; \
	fi;

clean:
	set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@ ; done
	-find . -name '*~' | xargs rm -f
