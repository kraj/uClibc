TOPDIR=../
include Rules.mak

SUBDIRS = util d-link libdl # man


all:
	@if [ -d d-link/$(TARGET_ARCH) ] ; then \
		set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d ; done \
	fi;

install: all
	@if [ -d d-link/$(TARGET_ARCH) ] ; then \
		install -d $(INSTALL_DIR)/lib
		install -d $(INSTALL_DIR)/etc
		install -m 755 ./d-link/$(DLINKER).$(LDSO_VMAJOR) $(INSTALL_DIR)/lib/
		install -m 644 ./libdl/$(LIBDL).$(LDSO_VMAJOR) $(INSTALL_DIR)/lib/
		(cd $(INSTALL_DIR)/lib/;ln -sf $(DLINKER).$(LDSO_VMAJOR) $(DLINKER))
		(cd $(INSTALL_DIR)/lib/;ln -sf $(LIBDL).$(LDSO_VMAJOR) $(LIBDL))
	fi;
d-link:
	@if [ -d d-link/$(TARGET_ARCH) ] ; then \
		do $(MAKE) -C d-link ; done \
	fi;

clean:
	set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@ ; done
	-find . -name '*~' | xargs rm -f
