# Makefile for uClibc
#
# Copyright (C) 2000 by Lineo, inc.
# Copyright (C) 2000-2002 Erik Andersen <andersen@uclibc.org>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Library General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for more
# details.
#
# You should have received a copy of the GNU Library General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
# Makefile for uClibc
#
# Derived in part from the Linux-8086 C library, the GNU C Library, and several
# other sundry sources.  Files within this library are copyright by their
# respective copyright holders.

#--------------------------------------------------------
#
#There are a number of configurable options in "Config"
#
#--------------------------------------------------------

TOPDIR=./
include Rules.mak

DIRS = extra ldso libc libcrypt libresolv libutil libm libpthread

all: headers uClibc_config subdirs shared utils finished

Config:
	@echo
	@echo "You didn't read the README, did you... =)"
	@echo "Choose a configuration file in extras/Config/ and then run"
	@echo "  ln -s ./extra/Configs/Config.<arch> ./Config"
	@echo
	@exit 1

shared:
ifeq ($(strip $(HAVE_SHARED)),true)
	@$(MAKE) -C libc shared
	@$(MAKE) -C ldso shared
	@$(MAKE) -C libcrypt shared
	@$(MAKE) -C libresolv shared
	@$(MAKE) -C libutil shared
	@$(MAKE) -C libm shared
	@$(MAKE) -C libpthread shared
else
	@echo
	@echo Not building shared libraries...
	@echo
endif

finished: shared
	@echo
	@echo Finally finished compiling...
	@echo

#
# Target for uClinux distro
#
romfs:
ifeq ($(strip $(HAVE_SHARED)),true)
	install -d $(ROMFSDIR)/lib
	install -m 644 lib/lib*-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
		$(ROMFSDIR)/lib
	cp -a lib/*.so.* $(ROMFSDIR)/lib
	@if [ -x lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so ] ; then \
	    set -x -e; \
	    install -m 755 lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
	    		$(ROMFSDIR)/lib; \
	fi;
endif

headers: dummy
	rm -f include/asm;
	@if [ $(TARGET_ARCH) = "powerpc" ];then \
	    ln -fs $(KERNEL_SOURCE)/include/asm-ppc include/asm; \
	elif [ $(TARGET_ARCH) = "mips" ];then \
	    ln -fs $(KERNEL_SOURCE)/include/asm-mips include/asm; \
	elif [ $(TARGET_ARCH) = "mipsel" ];then \
	    ln -fs $(KERNEL_SOURCE)/include/asm-mips include/asm; \
	    cd $(shell pwd)/libc/sysdeps/linux; \
	    ln -fs mips mipsel; \
	    cd $(shell pwd)/ldso/ldso; \
	    ln -fs mips mipsel; \
	    cd $(shell pwd)/libpthread/linuxthreads/sysdeps; \
	    ln -fs mips mipsel; \
	elif [ $(TARGET_ARCH) = "cris" ];then \
		ln -fs $(KERNEL_SOURCE)/include/asm-cris include/asm; \
	else \
	    if [ $(HAS_MMU) != "true" ]; then \
	    	if [ -d $(KERNEL_SOURCE)/include/asm-$(TARGET_ARCH)nommu ] ; then \
		    ln -fs $(KERNEL_SOURCE)/include/asm-$(TARGET_ARCH)nommu include/asm;\
		else \
		    ln -fs $(KERNEL_SOURCE)/include/asm-$(TARGET_ARCH) include/asm; \
		fi; \
	    else \
		ln -fs $(KERNEL_SOURCE)/include/asm-$(TARGET_ARCH) include/asm; \
	    fi; \
	fi;
	@if [ ! -f include/asm/unistd.h ] ; then \
	    set -e; \
	    echo " "; \
	    echo "The path '$(KERNEL_SOURCE)/include/asm' doesn't exist."; \
	    echo "I bet you didn't set KERNEL_SOURCE, TARGET_ARCH or HAS_MMU in \`Config'"; \
	    echo "correctly.  Please edit \`Config' and fix these settings."; \
	    echo " "; \
	    false; \
	fi;
	rm -f include/linux include/scsi
	ln -fs $(KERNEL_SOURCE)/include/linux include/linux
	ln -fs $(KERNEL_SOURCE)/include/scsi include/scsi
	rm -rf include/bits
	mkdir -p include/bits
	@cd include/bits; \
	set -e; \
	for i in `ls ../../libc/sysdeps/linux/common/bits/*.h` ; do \
		ln -fs $$i .; \
	done; \
	if [ -d ../../libc/sysdeps/linux/$(TARGET_ARCH)/bits ] ; then \
		for i in `ls ../../libc/sysdeps/linux/$(TARGET_ARCH)/bits/*.h` ; do \
			ln -fs $$i .; \
		done; \
	fi
	@cd include/sys; \
	set -e; \
	for i in `ls ../../libc/sysdeps/linux/common/sys/*.h` ; do \
		ln -fs $$i .; \
	done; \
	if [ -d ../../libc/sysdeps/linux/$(TARGET_ARCH)/sys ] ; then \
		for i in `ls ../../libc/sysdeps/linux/$(TARGET_ARCH)/sys/*.h` ; do \
			ln -fs $$i .; \
		done; \
	fi
	@cd $(TOPDIR); \
	set -x -e; \
	rm -f include/bits/sysnum.h; \
	TOPDIR=. CC=$(CC) /bin/sh extra/scripts/gen_bits_syscall_h.sh > include/bits/sysnum.h
	$(MAKE) -C libc/sysdeps/linux/$(TARGET_ARCH) headers

uClibc_config: Makefile Config
	@echo "/* WARNING!!! AUTO-GENERATED FILE!!! DO NOT EDIT!!! */" > include/bits/uClibc_config.h
	@echo "#if !defined __FEATURES_H && !defined __need_uClibc_config_h" >> include/bits/uClibc_config.h
	@echo "#error Never include <bits/uClibc_config.h> directly; use <features.h> instead." >> include/bits/uClibc_config.h
	@echo "#endif" >> include/bits/uClibc_config.h
	@echo "#define __UCLIBC_MAJOR__ $(MAJOR_VERSION)" >> include/bits/uClibc_config.h 
	@echo "#define __UCLIBC_MINOR__ $(MINOR_VERSION)" >> include/bits/uClibc_config.h 
	@echo "#define __UCLIBC_SUBLEVEL__ $(SUBLEVEL)" >> include/bits/uClibc_config.h 
	@echo "#define linux 1" >> include/bits/uClibc_config.h 
	@echo "#define __linux__ 1" >> include/bits/uClibc_config.h 
	@if [ "$(INCLUDE_IPV6)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_IPV6__ 1" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_IPV6__" >> include/bits/uClibc_config.h ; \
	fi
	@if [ "$(HAS_MMU)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_MMU__ 1" >> include/bits/uClibc_config.h ; \
	else \
	    echo "For now we make the assumption that a linux system without an"; \
	    echo "MMU must be uClinux, and must have a broken munmap (both"; \
	    echo "assumptions are a bit dodgy, but can be changed in the future)."; \
	    echo "#undef __UCLIBC_HAS_MMU__" >> include/bits/uClibc_config.h ; \
	    echo "#define __UCLIBC_UCLINUX_BROKEN_MUNMAP__ 1" >> include/bits/uClibc_config.h ; \
	fi
	@if [ "$(HAS_FLOATING_POINT)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_FLOATS__ 1" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_FLOATS__" >> include/bits/uClibc_config.h ; \
	fi
	@if [ "$(HAS_LOCALE)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_LOCALE__ 1" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_LOCALE__" >> include/bits/uClibc_config.h ; \
	fi
	@if [ "$(HAS_WCHAR)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_WCHAR__ 1" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_WCHAR__" >> include/bits/uClibc_config.h ; \
	fi
	@if [ "$(HAVE_ELF)" = "false" ] ; then \
	    echo "#undef HAVE_ELF" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#define HAVE_ELF 1" >> include/bits/uClibc_config.h ; \
	fi
	@if [ "$(HAVE_SHARED)" = "false" ] ; then \
	    echo "#undef HAVE_SHARED" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#define HAVE_SHARED 1" >> include/bits/uClibc_config.h ; \
	fi
	@if [ "$(TARGET_ARCH)" = "sh" ] ; then \
	    echo "#define NO_UNDERSCORES 1" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#undef NO_UNDERSCORES" >> include/bits/uClibc_config.h ; \
	fi
	@if [ "$(INCLUDE_RPC)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_RPC__ 1" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_RPC__" >> include/bits/uClibc_config.h ; \
	fi
	@if [ "$(DOLFS)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAVE_LFS__ 1" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAVE_LFS__" >> include/bits/uClibc_config.h ; \
	fi
	@if [ "$(INCLUDE_THREADS)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_THREADS__ 1" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_THREADS__" >> include/bits/uClibc_config.h ; \
	fi
	@if [ "$(INCLUDE_REGEX)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_REGEX__ 1" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_REGEX__" >> include/bits/uClibc_config.h ; \
	fi
	@if [ "$(UNIX98PTY_ONLY)" = "true" ] ; then \
	    echo "#define UNIX98PTY_ONLY 1" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#undef UNIX98PTY_ONLY" >> include/bits/uClibc_config.h ; \
	fi
	@if [ "$(ASSUME_DEVPTS)" = "true" ] ; then \
	    echo "#define ASSUME_DEVPTS 1" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#undef ASSUME_DEVPTS" >> include/bits/uClibc_config.h ; \
	fi
	@echo "#define C_SYMBOL_PREFIX "\""$(C_SYMBOL_PREFIX)"\" >> include/bits/uClibc_config.h
	@if [ "$(HAVE_DOT_HIDDEN)" = "true" ] ; then \
	    echo "#define HAVE_DOT_HIDDEN 1" >> include/bits/uClibc_config.h ; \
	else \
	    echo "#undef HAVE_DOT_HIDDEN" >> include/bits/uClibc_config.h ; \
	fi

subdirs: $(patsubst %, _dir_%, $(DIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

tags:
	ctags -R

install: install_dev install_runtime install_toolchain install_utils finished2


# Installs header files and development library links.
install_dev:
	install -d $(PREFIX)$(DEVEL_PREFIX)/lib
	install -d $(PREFIX)$(DEVEL_PREFIX)/usr/lib
	install -d $(PREFIX)$(DEVEL_PREFIX)/include
	-install -m 644 lib/*.[ao] $(PREFIX)$(DEVEL_PREFIX)/lib/
	tar -chf - include | tar -xf - -C $(PREFIX)$(DEVEL_PREFIX);
	-@for i in `find  $(PREFIX)$(DEVEL_PREFIX) -type d` ; do \
	    chmod -f 755 $$i; chmod -f 644 $$i/*.h; \
	done;
	-find $(PREFIX)$(DEVEL_PREFIX) -name CVS | xargs rm -rf;
	-chown -R `id | sed 's/^uid=\([0-9]*\).*gid=\([0-9]*\).*$$/\1.\2/'` $(PREFIX)$(DEVEL_PREFIX)
ifeq ($(strip $(HAVE_SHARED)),true)
	-find lib/ -type l -name '*.so' -exec cp -a {} $(PREFIX)$(DEVEL_PREFIX)/lib ';'
	# If we build shared libraries then the static libs are PIC...
	# Make _pic.a symlinks to make mklibs.py and similar tools happy.
	for i in `find lib/  -type f -name '*.a' | sed -e 's/lib\///'` ; do \
		ln -sf $$i $(PREFIX)$(DEVEL_PREFIX)/lib/`echo $$i | sed -e 's/\.a$$/_pic.a/'`; \
	done
endif


# Installs run-time libraries and helper apps onto the host system
# allowing cross development.  If you want to deploy to a target 
# system, use the "install_target" target instead... 
install_runtime:
ifeq ($(strip $(HAVE_SHARED)),true)
	install -d $(PREFIX)$(DEVEL_PREFIX)/lib
	install -d $(PREFIX)$(DEVEL_PREFIX)/bin
	install -m 644 lib/lib*-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
		$(PREFIX)$(DEVEL_PREFIX)/lib
	cp -a lib/*.so.* $(PREFIX)$(DEVEL_PREFIX)/lib
	@if [ -x lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so ] ; then \
	    set -x -e; \
	    install -m 755 lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
	    		$(PREFIX)$(DEVEL_PREFIX)/lib; \
	    mkdir -p $(PREFIX)$(SHARED_LIB_LOADER_PATH); \
	    ln -s $(DEVEL_PREFIX)/lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
			$(PREFIX)$(SHARED_LIB_LOADER_PATH)/$(UCLIBC_LDSO) || true; \
	fi;
endif

install_toolchain:
	install -d $(PREFIX)$(DEVEL_PREFIX)/lib
	install -d $(PREFIX)$(DEVEL_PREFIX)/bin
	install -d $(PREFIX)$(DEVEL_TOOL_PREFIX)/bin
	install -d $(PREFIX)$(SYSTEM_DEVEL_PREFIX)/bin
	$(MAKE) -C extra/gcc-uClibc install

ifeq ($(strip $(HAVE_SHARED)),true)
utils: $(TOPDIR)ldso/util/ldd
	$(MAKE) -C ldso utils
else
utils: dummy
endif

install_utils: utils
ifeq ($(strip $(HAVE_SHARED)),true)
	install -d $(PREFIX)$(DEVEL_TOOL_PREFIX)/bin;
	install -m 755 ldso/util/ldd \
		$(PREFIX)$(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-ldd
	ln -fs $(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-ldd \
		$(PREFIX)$(DEVEL_TOOL_PREFIX)/bin/ldd
	# For now, don't bother with readelf since surely the host
	# system has binutils, or we couldn't have gotten this far...
	#install -m 755 ldso/util/readelf \
	#	$(PREFIX)$(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-readelf
	#ln -fs $(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-readelf \
	#	$(PREFIX)$(DEVEL_TOOL_PREFIX)/bin/readelf
	@if [ -x ldso/util/ldconfig ] ; then \
	    set -x -e; \
	    install -d $(PREFIX)$(DEVEL_PREFIX)/etc; \
	    install -m 755 ldso/util/ldconfig \
		    $(PREFIX)$(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-ldconfig; \
	    ln -fs $(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-ldconfig \
		    $(PREFIX)$(DEVEL_TOOL_PREFIX)/bin/ldconfig; \
	fi;
endif

# Installs run-time libraries and helper apps in preparation for
# deploying onto a target system, but installed below wherever
# $PREFIX is set to, allowing you to package up the result for
# deployment onto your target system.
install_target:
ifeq ($(strip $(HAVE_SHARED)),true)
	install -d $(PREFIX)$(TARGET_PREFIX)/lib
	install -d $(PREFIX)$(TARGET_PREFIX)/usr/bin
	install -m 644 lib/lib*-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
		$(PREFIX)$(TARGET_PREFIX)/lib
	cp -a lib/*.so.* $(PREFIX)$(TARGET_PREFIX)/lib
	@if [ -x lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so ] ; then \
	    set -x -e; \
	    install -m 755 lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
	    		$(PREFIX)$(TARGET_PREFIX)/lib; \
	    mkdir -p $(PREFIX)$(SHARED_LIB_LOADER_PATH); \
	    ln -s $(TARGET_PREFIX)/lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
	    		$(PREFIX)$(SHARED_LIB_LOADER_PATH)/$(UCLIBC_LDSO) || true; \
	fi;
endif

install_target_utils:
ifeq ($(strip $(HAVE_SHARED)),true)
	@$(MAKE) -C ldso/util ldd.target readelf.target #ldconfig.target
	install -d $(PREFIX)$(TARGET_PREFIX)/usr/bin;
	install -m 755 ldso/util/ldd.target $(PREFIX)$(TARGET_PREFIX)/usr/bin/ldd
	install -m 755 ldso/util/readelf.target $(PREFIX)$(TARGET_PREFIX)/usr/bin/readelf
	@if [ -x ldso/util/ldconfig.target ] ; then \
	    set -x -e; \
	    install -d $(PREFIX)$(TARGET_PREFIX)/etc; \
	    install -d $(PREFIX)$(TARGET_PREFIX)/sbin; \
	    install -m 755 ldso/util/ldconfig.target $(PREFIX)$(TARGET_PREFIX)/sbin/ldconfig; \
	fi;
endif

finished2:
	@echo
	@echo Finished installing...
	@echo


distclean clean:
	- find . \( -name \*.o -o -name \*.a -o -name \*.so -o -name core -o -name .\#\* \) -exec rm -f {} \;
	@rm -rf tmp lib include/bits libc/tmp _install
	$(MAKE) -C test clean
	$(MAKE) -C ldso clean
	$(MAKE) -C libc/misc/internals clean
	$(MAKE) -C libc/unistd clean
	$(MAKE) -C libc/sysdeps/linux/common clean
	$(MAKE) -C extra/gcc-uClibc clean
	@set -e; \
	for i in `(cd $(TOPDIR)/libc/sysdeps/linux/common/sys; ls *.h)` ; do \
		rm -f include/sys/$$i; \
	done; \
	if [ -d libc/sysdeps/linux/$(TARGET_ARCH)/sys ] ; then \
		for i in `(cd libc/sysdeps/linux/$(TARGET_ARCH)/sys; ls *.h)` ; do \
			rm -f include/sys/$$i; \
		done; \
	fi;
	@rm -f include/linux include/scsi include/asm
	@if [ -d libc/sysdeps/linux/$(TARGET_ARCH) ]; then		\
	    $(MAKE) -C libc/sysdeps/linux/$(TARGET_ARCH) clean;		\
	fi;
	@if [ $(TARGET_ARCH) = "mipsel" ]; then \
	    $(MAKE) -C libc/sysdeps/linux/mips clean; \
	    rm -f ldso/ldso/mipsel; \
	    rm -f libc/sysdeps/linux/mipsel; \
	    rm -f libpthread/linuxthreads/sysdeps/mipsel; \
	fi;

dist release: distclean
	cd ..;					\
	rm -rf uClibc-$(VERSION);		\
	cp -a uClibc uClibc-$(VERSION);		\
	find uClibc-$(VERSION)/ -type d		\
	    -name .\#* -exec rm -rf {} \; ;	\
						\
	tar -cvzf uClibc-$(VERSION).tar.gz --exclude CVS uClibc-$(VERSION)/;

.PHONY: dummy subdirs release distclean clean

