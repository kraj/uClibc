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

ifndef $(TARGET_PREFIX)
	TARGET_PREFIX = `pwd`/_install
endif

all: headers uClibc_config.h subdirs $(DO_SHARED) done

Config:
	@echo
	@echo "You didn't read the README, did you... =)"
	@echo "Choose a configuration file in extras/Config/ and copy it to ./Config."
	@echo
	@exit 1

shared:
	@$(MAKE) -C libc shared
	@$(MAKE) -C ldso/util
ifeq ($(LDSO_PRESENT), $(TARGET_ARCH))
	@$(MAKE) -C ldso shared
endif
	@$(MAKE) -C libcrypt shared
	@$(MAKE) -C libutil shared
	@$(MAKE) -C libm shared
	@$(MAKE) -C libresolv shared

done: $(DO_SHARED)
	@echo
	@echo Finally finished compiling...
	@echo

headers: dummy
	@rm -f include/asm include/linux include/bits
	@if [ $(TARGET_ARCH) = "powerpc" ];then \
	    ln -s $(KERNEL_SOURCE)/include/asm-ppc include/asm; \
	else \
	    if [ $(HAS_MMU) != "true" ]; then \
			ln -s $(KERNEL_SOURCE)/include/asm-$(TARGET_ARCH)nommu include/asm;\
		else \
			ln -s $(KERNEL_SOURCE)/include/asm-$(TARGET_ARCH) include/asm; \
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
	@ln -s $(KERNEL_SOURCE)/include/linux include/linux
	@ln -s ../libc/sysdeps/linux/$(TARGET_ARCH)/bits include/bits
	(cd include/bits; ln -sf ../../../../../uClibc_config.h uClibc_config.h)
	$(MAKE) -C libc/sysdeps/linux/$(TARGET_ARCH) headers

uClibc_config.h: Makefile Config
	@echo "/* WARNING!!! AUTO-GENERATED FILE!!! DO NOT EDIT!!! */" > uClibc_config.h
	@echo "#if !defined __FEATURES_H && !defined __need_uClibc_config_h" >> uClibc_config.h
	@echo "#error Never include <bits/uClibc_config.h> directly; use <features.h> instead." >> uClibc_config.h
	@echo "#endif" >> uClibc_config.h
	@echo "#define linux 1" >> uClibc_config.h 
	@echo "#define __linux__ 1" >> uClibc_config.h 
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
	@if [ "$(HAS_LIBM_FLOAT)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_LIBM_FLOAT__ 1" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_LIBM_FLOAT__" >> uClibc_config.h ; \
	fi
	@if [ "$(HAS_LIBM_DOUBLE)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_LIBM_DOUBLE__ 1" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_LIBM_DOUBLE__" >> uClibc_config.h ; \
	fi
	@if [ "$(HAS_LIBM_LONG_DOUBLE)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_LIBM_LONG_DOUBLE__ 1" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_LIBM_LONG_DOUBLE__" >> uClibc_config.h ; \
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

subdirs: $(patsubst %, _dir_%, $(DIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

tags:
	ctags -R

install: install_dev

# Installs shared libraries for a target.
install_target:
ifeq ($(DO_SHARED),shared)
	install -d $(TARGET_PREFIX)$(ROOT_DIR)/lib
	cp -fa lib/*.so* $(TARGET_PREFIX)$(ROOT_DIR)/lib;
	install -d $(TARGET_PREFIX)$(ROOT_DIR)/etc
	install -d $(TARGET_PREFIX)$(ROOT_DIR)/sbin
	install -d $(TARGET_PREFIX)$(ROOT_DIR)/usr/bin
	cp -f ldso/util/ldd $(TARGET_PREFIX)$(ROOT_DIR)/usr/bin
	cp -f ldso/util/ldconfig $(TARGET_PREFIX)$(ROOT_DIR)/sbin
ifeq ($(NATIVE_ARCH), $(TARGET_ARCH))
	-@if [ -x ldso/util/ldconfig ] ; then ldso/util/ldconfig; fi
endif
endif

# Installs development library and headers
# This is done with the assumption that it can blow away anything
# in $(DEVEL_PREFIX)$(ROOT_DIR)/include.  Probably true only if you're using
# a packaging system.
install_dev:
	install -d $(DEVEL_PREFIX)$(ROOT_DIR)/usr/lib
	cp -fa lib/*.[ao] $(DEVEL_PREFIX)$(ROOT_DIR)/usr/lib;
ifeq ($(DO_SHARED),shared)
	install -d $(DEVEL_PREFIX)$(ROOT_DIR)/lib
	cp -fa lib/*.so* $(DEVEL_PREFIX)$(ROOT_DIR)/lib;
	install -d $(DEVEL_PREFIX)$(ROOT_DIR)/etc
	install -d $(DEVEL_PREFIX)$(ROOT_DIR)/sbin
	install -d $(DEVEL_PREFIX)$(ROOT_DIR)/usr/bin
	cp -f ldso/util/ldd $(DEVEL_PREFIX)$(ROOT_DIR)/usr/bin
	cp -f ldso/util/ldconfig $(DEVEL_PREFIX)$(ROOT_DIR)/sbin
ifeq ($(NATIVE_ARCH), $(TARGET_ARCH))
	-@if [ -x ldso/util/ldconfig ] ; then ldso/util/ldconfig; fi
endif
endif
	install -d $(DEVEL_PREFIX)$(ROOT_DIR)/etc
	install -d $(DEVEL_PREFIX)$(ROOT_DIR)/usr/include
	install -d $(DEVEL_PREFIX)$(ROOT_DIR)/usr/include/bits
	rm -f $(DEVEL_PREFIX)$(ROOT_DIR)/usr/include/asm
	rm -f $(DEVEL_PREFIX)$(ROOT_DIR)/usr/include/linux
	ln -s $(KERNEL_SOURCE)/include/asm $(DEVEL_PREFIX)$(ROOT_DIR)/usr/include/asm
	ln -s $(KERNEL_SOURCE)/include/linux $(DEVEL_PREFIX)$(ROOT_DIR)/usr/include/linux
	find include/ -type f -depth -not -path "*CVS*" -exec install \
	    -D -m 644 {} $(DEVEL_PREFIX)$(ROOT_DIR)/usr/'{}' ';'
	find include/bits/ -type f -depth -not -path "*CVS*" -exec install \
	    -D -m 644 {} $(DEVEL_PREFIX)$(ROOT_DIR)/usr/'{}' ';'
	install -m 644 include/bits/uClibc_config.h $(DEVEL_PREFIX)$(ROOT_DIR)/usr/include/bits/
	$(MAKE) -C extra/gcc-uClibc install

clean:
	@rm -rf tmp lib include/bits/uClibc_config.h uClibc_config.h
	- find include -type l -exec rm -f {} \;
	- find . \( -name \*.o -o -name \*.a -o -name \*.so -o -name core \) -exec rm -f {} \;
	$(MAKE) -C ldso clean
	$(MAKE) -C libc/unistd clean
	$(MAKE) -C libc/sysdeps/linux/common clean
	$(MAKE) -C libc/sysdeps/linux/$(TARGET_ARCH) clean
	$(MAKE) -C extra/gcc-uClibc clean

.PHONY: dummy subdirs

