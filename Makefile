# Makefile for uClibc
#
# Copyright (C) 2000, 2001 by Lineo, inc.
# Written by Erik Andersen <andersen@lineo.com>, <andersee@debian.org>
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

DIRS = extra ldso libc libcrypt libresolv libutil libm  

all: headers uClibc_config.h subdirs shared done

Config:
	@echo
	@echo "You didn't read the README, did you... =)"
	@echo "Choose a configuration file in extras/Config/ and copy it to ./Config"
	@echo
	@exit 1

shared:
ifeq ($(strip $(HAVE_SHARED)),true)
	@$(MAKE) -C libc shared
	@$(MAKE) -C ldso shared
	@$(MAKE) -C libcrypt shared
	@$(MAKE) -C libutil shared
	@$(MAKE) -C libm shared
	@$(MAKE) -C libresolv shared
else
	@echo
	@echo Not building shared libraries...
	@echo
endif

done: shared
	@echo
	@echo Finally finished compiling...
	@echo

headers: dummy
	@rm -f include/asm include/linux include/bits
	@if [ $(TARGET_ARCH) = "powerpc" ];then \
	    ln -fs $(KERNEL_SOURCE)/include/asm-ppc include/asm; \
	elif [ $(TARGET_ARCH) = "v850" ];then \
	    ln -fs $(KERNEL_SOURCE)/include/asm-v850 include/asm; \
	else \
	    if [ $(HAS_MMU) != "true" ]; then \
			ln -fs $(KERNEL_SOURCE)/include/asm-$(TARGET_ARCH)nommu include/asm;\
		else \
			ln -fs $(KERNEL_SOURCE)/include/asm-$(TARGET_ARCH) include/asm; \
		fi; \
	fi;
	@if [ ! -f include/asm/unistd.h ] ; then \
	    echo " "; \
	    echo "The path '$(KERNEL_SOURCE)/include/asm' doesn't exist."; \
	    echo "I bet you didn't set KERNEL_SOURCE, TARGET_ARCH or HAS_MMU in \`Config'"; \
	    echo "correctly.  Please edit \`Config' and fix these settings."; \
	    echo " "; \
	    /bin/false; \
	fi;
	@if [ $(HAS_MMU) != "true" ]  && [ $(TARGET_ARCH) = "i386" ] ; then \
	    echo "WARNING: I bet your x86 system really has an MMU, right?"; \
	    echo "         malloc and friends won't work unless you fix \`Config'"; \
	    echo " "; \
	    sleep 10; \
	fi;
	@ln -fs $(KERNEL_SOURCE)/include/linux include/linux
	@ln -fs ../libc/sysdeps/linux/$(TARGET_ARCH)/bits include/bits
	(cd include/bits; ln -sf ../../../../../uClibc_config.h uClibc_config.h)
	$(MAKE) -C libc/sysdeps/linux/$(TARGET_ARCH) headers

uClibc_config.h: Makefile Config
	@echo "/* WARNING!!! AUTO-GENERATED FILE!!! DO NOT EDIT!!! */" > uClibc_config.h
	@echo "#if !defined __FEATURES_H && !defined __need_uClibc_config_h" >> uClibc_config.h
	@echo "#error Never include <bits/uClibc_config.h> directly; use <features.h> instead." >> uClibc_config.h
	@echo "#endif" >> uClibc_config.h
	@echo "#define linux 1" >> uClibc_config.h 
	@echo "#define __linux__ 1" >> uClibc_config.h 
	@if [ "$(INCLUDE_IPV6)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_IPV6__ 1" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_IPV6__" >> uClibc_config.h ; \
	fi
	@if [ "$(HAS_MMU)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_MMU__ 1" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_MMU__" >> uClibc_config.h ; \
	fi
	@if [ "$(HAS_FLOATING_POINT)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_FLOATS__ 1" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_FLOATS__" >> uClibc_config.h ; \
	fi
	@if [ "$(HAS_LONG_LONG)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_LONG_LONG__ 1" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_LONG_LONG__" >> uClibc_config.h ; \
	fi
	@if [ "$(HAS_LOCALE)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_LOCALE__ 1" >> uClibc_config.h ; \
	    echo "#define __UCLIBC_LOCALE_DIR \""$(LOCALE_DIR)"\"" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_LOCALE__" >> uClibc_config.h ; \
	fi
	@if [ "$(HAVE_ELF)" = "false" ] ; then \
	    echo "#undef HAVE_ELF" >> uClibc_config.h ; \
	else \
	    echo "#define HAVE_ELF 1" >> uClibc_config.h ; \
	fi
	@if [ "$(HAVE_SHARED)" = "false" ] ; then \
	    echo "#undef HAVE_SHARED" >> uClibc_config.h ; \
	else \
	    echo "#define HAVE_SHARED 1" >> uClibc_config.h ; \
	fi
	@if [ "$(TARGET_ARCH)" = "sh" ] ; then \
	    echo "#define NO_UNDERSCORES 1" >> uClibc_config.h ; \
	else \
	    echo "#undef NO_UNDERSCORES" >> uClibc_config.h ; \
	fi
	@if [ "$(INCLUDE_RPC)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_RPC__ 1" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_RPC__" >> uClibc_config.h ; \
	fi
	@if [ "$(UNIFIED_SYSCALL)" = "true" ] ; then \
	    echo "#define __UCLIBC_USE_UNIFIED_SYSCALL__ 1" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_USE_UNIFIED_SYSCALL__" >> uClibc_config.h ; \
	fi
	@echo "#define C_SYMBOL_PREFIX "\""$(C_SYMBOL_PREFIX)"\" >> uClibc_config.h
	@if [ "$(DOLFS)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAVE_LFS__ 1" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAVE_LFS__" >> uClibc_config.h ; \
	fi

subdirs: $(patsubst %, _dir_%, $(DIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

tags:
	ctags -R

install: install_dev install_runtime install_gcc


# Installs header files and development library links.
install_dev:
	install -d $(PREFIX)$(DEVEL_PREFIX)/lib
	install -m 644 lib/*.[ao] $(PREFIX)$(DEVEL_PREFIX)/lib/
	install -d $(PREFIX)$(DEVEL_PREFIX)/include
	install -d $(PREFIX)$(DEVEL_PREFIX)/usr/lib
	find include/ -name '*.h' -depth -follow -exec install \
	    -D -m 644 {} $(PREFIX)$(DEVEL_PREFIX)/'{}' ';'
ifeq ($(strip $(HAVE_SHARED)),true)
	find lib/ -type l -name '*.so' -exec cp -a {} $(PREFIX)$(DEVEL_PREFIX)/lib ';'
endif


# Installs run-time libraries and helper apps onto the host system
# allowing cross development.  If you want to deploy to a target 
# system, use the "install_target" target instead... 
install_runtime:
ifeq ($(strip $(HAVE_SHARED)),true)
	install -d $(PREFIX)$(DEVEL_PREFIX)/lib
	install -d $(PREFIX)$(DEVEL_PREFIX)/bin
	install -m 644 lib/lib*-$(MAJOR_VERSION).$(MINOR_VERSION).so $(PREFIX)$(DEVEL_PREFIX)/lib
	cp -a lib/*.so.* $(PREFIX)$(DEVEL_PREFIX)/lib
	install -m 755 ldso/util/ldd $(PREFIX)$(DEVEL_PREFIX)/bin
	install -m 755 ldso/util/readelf $(PREFIX)$(DEVEL_PREFIX)/bin
	install -d $(PREFIX)$(SYSTEM_DEVEL_PREFIX)/bin
	ln -fs $(DEVEL_PREFIX)/bin/ldd $(PREFIX)$(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-ldd
	ln -fs $(DEVEL_PREFIX)/bin/readelf $(PREFIX)$(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-readelf
	if [ -x lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).so ] ; then \
	    install -m 755 lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).so $(PREFIX)$(DEVEL_PREFIX)/lib; \
	    mkdir -p $(PREFIX)$(SHARED_LIB_LOADER_PATH); \
	    ln -s $(DEVEL_PREFIX)/lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).so \
	    		$(PREFIX)$(SHARED_LIB_LOADER_PATH)/$(UCLIBC_LDSO) || true; \
	fi;
	if [ -x ldso/util/ldconfig ] ; then \
	    install -d $(PREFIX)$(DEVEL_PREFIX)/etc; \
	    install -m 755 ldso/util/ldconfig $(PREFIX)$(DEVEL_PREFIX)/bin; \
	    ln -fs $(DEVEL_PREFIX)/sbin/ldconfig $(PREFIX)$(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-ldconfig; \
	fi;
endif

install_gcc:
	$(MAKE) -C extra/gcc-uClibc install


# Installs run-time libraries and helper apps in preparation for
# deploying onto a target system, but installed below wherever
# $PREFIX is set to, allowing you to package up the result for
# deployment onto your target system.
install_target:
ifeq ($(strip $(HAVE_SHARED)),true)
	install -d $(PREFIX)$(TARGET_PREFIX)/lib
	install -d $(PREFIX)$(TARGET_PREFIX)/sbin
	install -d $(PREFIX)$(TARGET_PREFIX)/usr/bin
	install -m 644 lib/lib*-$(MAJOR_VERSION).$(MINOR_VERSION).so $(PREFIX)$(TARGET_PREFIX)/lib
	cp -a lib/*.so.* $(PREFIX)$(TARGET_PREFIX)/lib
	install -m 755 ldso/util/ldd $(PREFIX)$(TARGET_PREFIX)/usr/bin
	install -m 755 ldso/util/readelf $(PREFIX)$(TARGET_PREFIX)/usr/bin
	if [ -x lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).so ] ; then \
	    install -m 755 lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).so $(PREFIX)$(TARGET_PREFIX)/lib; \
	    mkdir -p $(PREFIX)$(SHARED_LIB_LOADER_PATH); \
	    ln -s $(TARGET_PREFIX)/lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).so \
	    		$(PREFIX)$(SHARED_LIB_LOADER_PATH)/$(UCLIBC_LDSO) || true; \
	fi;
	if [ -x ldso/util/ldconfig ] ; then \
	    install -d $(PREFIX)$(TARGET_PREFIX)/etc; \
	    install -m 755 ldso/util/ldconfig $(PREFIX)$(TARGET_PREFIX)/sbin; \
	fi;
endif

distclean clean:
	@rm -rf tmp lib include/bits/uClibc_config.h uClibc_config.h
	- find include -type l -exec rm -f {} \;
	- find . \( -name \*.o -o -name \*.a -o -name \*.so -o -name core -o -name .\#\* \) -exec rm -f {} \;
	$(MAKE) -C test clean
	$(MAKE) -C ldso clean
	$(MAKE) -C libc/unistd clean
	$(MAKE) -C libc/sysdeps/linux/common clean
	$(MAKE) -C libc/sysdeps/linux/$(TARGET_ARCH) clean
	$(MAKE) -C extra/gcc-uClibc clean

dist release: distclean
	cd ..;					\
	rm -rf uClibc-$(VERSION);		\
	cp -a uClibc uClibc-$(VERSION);		\
						\
	find uClibc-$(VERSION)/ -type d	\
				 -name CVS	\
		-exec rm -rf {} \; ;            \
						\
	tar -cvzf uClibc-$(VERSION).tar.gz --exclude CVS uClibc-$(VERSION)/;

.PHONY: dummy subdirs

