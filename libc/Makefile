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

TOPDIR=../
include $(TOPDIR)Rules.mak

DIRS = misc pwd_grp stdio string termios inet signal stdlib sysdeps unistd

all: halfclean subdirs $(LIBNAME) $(DO_SHARED) done

$(LIBNAME): subdirs
	$(CROSS)ranlib $(LIBNAME)
	install -d $(TOPDIR)lib
	rm -f $(TOPDIR)lib/$(LIBNAME)
	install -m 644 crt0.o $(LIBNAME) $(TOPDIR)lib

shared: $(TOPDIR)lib/$(LIBNAME)
	@rm -rf tmp
	@mkdir tmp
	@(cd tmp; CC=$(CC) LD=$(LD) /bin/sh ../../extra/scripts/get-needed-libgcc-objects.sh)
	if [ -s ./tmp/libgcc-need.a ] ; then \
		$(CC) -g $(LDFLAGS) -shared -o $(SHARED_FULLNAME) \
		    -Wl,-soname,$(SHARED_MAJORNAME) -Wl,--whole-archive \
		    ./$(LIBNAME) ./tmp/libgcc-need.a \
		    $(LDSO) -Wl,--dynamic-linker,$(DYNAMIC_LINKER); \
	else \
		$(CC) -g $(LDFLAGS) -shared -o $(SHARED_FULLNAME) \
		    -Wl,-soname,$(SHARED_MAJORNAME) -Wl,--whole-archive \
		    ./$(LIBNAME) $(LDSO)  -Wl,-dynamic-linker,$(DYNAMIC_LINKER); \
	fi
	@rm -rf tmp
	install -d $(TOPDIR)lib
	rm -f $(TOPDIR)lib/$(SHARED_FULLNAME)
	install -m 644 $(SHARED_FULLNAME) $(TOPDIR)lib
	(cd ../lib; ln -sf $(SHARED_FULLNAME) libc.so);
	(cd ../lib; ln -sf $(SHARED_FULLNAME) $(SHARED_MAJORNAME));


done: $(LIBNAME) $(DO_SHARED)
	@echo
	@echo Finally finished compiling...
	@echo

halfclean:
	@rm -f $(LIBNAME) crt0.o uClibc_config.h
	@rm -f $(SHARED_FULLNAME) $(SHARED_MAJORNAME) uClibc-0.* libc.so*

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
	@ln -s ../sysdeps/linux/$(TARGET_ARCH)/bits include/bits
	@ln -sf ../../../../uClibc_config.h sysdeps/linux/$(TARGET_ARCH)/bits/uClibc_config.h


tags:
	ctags -R

clean: subdirs_clean halfclean
	@rm -rf tmp
	rm -f include/asm include/linux include/bits

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(DIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

.PHONY: dummy subdirs

