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

DIRS = extra ldso libc libcrypt libutil libm

all: headers uClibc_config.h subdirs $(DO_SHARED) done

shared:
	@$(MAKE) -C libc shared
	@$(MAKE) -C ldso shared
	@$(MAKE) -C libcrypt shared
	@$(MAKE) -C libutil shared
	@$(MAKE) -C libm shared

done: $(DO_SHARED)
	@echo
	@echo Finally finished compiling...
	@echo

headers: dummy
	@rm -f include/asm include/linux include/bits
	@ln -s $(KERNEL_SOURCE)/include/asm include/asm
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

uClibc_config.h: Config
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
	@if [ "$(HAS_FLOATS)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_FLOATS__ 1" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_FLOATS__" >> uClibc_config.h ; \
	fi
	@if [ "$(HAS_DOUBLE)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_DOUBLE__ 1" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_DOUBLE__" >> uClibc_config.h ; \
	fi
	@if [ "$(HAS_LONG_DOUBLE)" = "true" ] ; then \
	    echo "#define __UCLIBC_HAS_LONG_DOUBLE__ 1" >> uClibc_config.h ; \
	else \
	    echo "#undef __UCLIBC_HAS_LONG_DOUBLE__" >> uClibc_config.h ; \
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
	@if [ "$(TARGET_ARCH)" = "m68k" ] ; then \
	    echo "#define __VFORK_MACRO__ 1" >> uClibc_config.h ; \
	    if [ `expr match "$(CC)" ".*\(m68k-elf-.*\)"`x = x ]; then \
		echo "#define const" >> uClibc_config.h ; \
		echo "#define __const" >> uClibc_config.h ; \
		echo "#define __extension" >> uClibc_config.h ; \
	    fi; \
	else \
	    echo "#undef __VFORK_MACRO__" >> uClibc_config.h ; \
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

subdirs: $(patsubst %, _dir_%, $(DIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

tags:
	ctags -R

install: install_runtime install_dev

# Installs shared library
install_runtime:
	install -d $(INSTALL_DIR)/lib
	cp -fa lib/* $(INSTALL_DIR)/lib;

# Installs development library and headers
# This is done with the assumption that it can blow away anything
# in $(INSTALL_DIR)/include.  Probably true only if you're using
# a packaging system.
install_dev:
	install -d $(INSTALL_DIR)/include
	install -d $(INSTALL_DIR)/include/bits
	rm -f $(INSTALL_DIR)/include/asm
	rm -f $(INSTALL_DIR)/include/linux
	ln -s $(KERNEL_SOURCE)/include/asm $(INSTALL_DIR)/include/asm
	ln -s $(KERNEL_SOURCE)/include/linux $(INSTALL_DIR)/include/linux
	find include/ -type f -depth -not -path "*CVS*" -exec install \
	    -D -m 644 {} $(INSTALL_DIR)/'{}' ';'
	find include/bits/ -type f -depth -not -path "*CVS*" -exec install \
	    -D -m 644 {} $(INSTALL_DIR)/'{}' ';'
	install -m 644 include/bits/uClibc_config.h $(INSTALL_DIR)/include/bits/
	$(MAKE) -C extra/gcc-uClibc install

clean:
	@rm -rf tmp lib
	rm -f include/asm include/linux include/bits uClibc_config.h
	- find . \( -name \*.o -o -name \*.a -o -name \*.so -o -name core \) -exec rm -f {} \;

.PHONY: dummy subdirs

