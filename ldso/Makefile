TOPDIR=../
include Rules.mak

SUBDIRS = util d-link libdl # man


all:
	set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d ; done

install: all
	install -d $(INSTALL_DIR)/lib
	install -d $(INSTALL_DIR)/etc
	install -m 755 ./d-link/$(DLINKER).$(LDSO_VMAJOR) $(INSTALL_DIR)/lib/
	install -m 644 ./libdl/$(LIBDL).$(LDSO_VMAJOR) $(INSTALL_DIR)/lib/
	(cd $(INSTALL_DIR)/lib/;ln -sf $(DLINKER).$(LDSO_VMAJOR) $(DLINKER))
	(cd $(INSTALL_DIR)/lib/;ln -sf $(LIBDL).$(LDSO_VMAJOR) $(LIBDL))

clean:
	set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@ ; done
	-find . -name '*~' | xargs rm -f
