# Makefile for uClibc
#
# Copyright (C) 2000 by Lineo, inc.
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

include Rules.mak


DIRS = extra misc pwd_grp stdio string termios inet signal stdlib sysdeps unistd

ifeq ($(strip $(HAS_MMU)),true)
	DO_SHARED=shared
endif

all: $(STATIC_NAME) $(DO_SHARED) done

$(STATIC_NAME): halfclean headers subdirs
	$(CROSS)ranlib $(STATIC_NAME)

# Surely there is a better way to do this then dumping all 
# the objects into a tmp dir.  Please -- someone enlighten me.
shared: $(STATIC_NAME)
	@rm -rf tmp
	@mkdir tmp
	@(cd tmp; ar -x ../$(STATIC_NAME))
	@(cd tmp; CC=$(CC) /bin/sh ../extra/scripts/get-needed-libgcc-objects.sh)
	$(CC) -s -nostdlib -shared -o $(SHARED_NAME) -Wl,-soname,$(SHARED_NAME) tmp/*.o
	@rm -rf tmp

done: $(STATIC_NAME) $(DO_SHARED)
	@echo
	@echo Finally finished compiling...
	@echo

halfclean:
	@rm -f $(STATIC_NAME) $(SHARED_NAME) crt0.o

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

tags:
	ctags -R

clean: subdirs_clean
	@rm -rf tmp
	rm -f $(STATIC_NAME) crt0.o $(SHARED_NAME)
	rm -f include/asm include/linux include/bits

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(DIRS) test)

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(DIRS) test) : dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

install:
	@if [ `id -u` -ne 0 ]; then \
	    echo "Aborting install -- You must be root."; \
	    /bin/false; \
	fi;
	@if [ -n "$(DO_SHARED)" ] ; then \
	    set -x; \
	    mv -f $(INSTALL_DIR)/lib/$(SHARED_NAME) \
		$(INSTALL_DIR)/lib/$(SHARED_NAME).old > /dev/null 2>&1; \
	    rm -f $(INSTALL_DIR)/lib/$(SHARED_NAME).old; \
	    cp $(SHARED_NAME) $(INSTALL_DIR)/lib; \
	    chmod 644 $(INSTALL_DIR)/lib/$(SHARED_NAME); \
	    chown -R root.root $(INSTALL_DIR)/lib/$(SHARED_NAME); \
	    rm -f $(INSTALL_DIR)/lib/libuClibc.so; \
	    ln -s $(INSTALL_DIR)/lib/$(SHARED_NAME) \
		    $(INSTALL_DIR)/lib/libuClibc.so; \
	    ldconfig; \
	fi;
	@if [ "$(HAS_MMU)" = "false" ] ; then \
	    set -x; \
	    rm -f $(INSTALL_DIR)/include/asm; \
	    rm -f $(INSTALL_DIR)/include/linux; \
	    mkdir -p $(INSTALL_DIR)/include/bits; \
	    ln -s $(KERNEL_SOURCE)/include/asm $(INSTALL_DIR)/include/asm; \
	    ln -s $(KERNEL_SOURCE)/include/linux $(INSTALL_DIR)/include/linux; \
	    find include/ -type f -depth -print | cpio -pdmu $(INSTALL_DIR); \
	    find include/bits/ -depth -print | cpio -pdmu $(INSTALL_DIR); \
	    rm -f $(INSTALL_DIR)/lib/$(STATIC_NAME); \
	    cp $(STATIC_NAME) $(INSTALL_DIR)/lib; \
	    chmod 644 $(INSTALL_DIR)/lib/$(STATIC_NAME); \
	    chown -R root.root $(INSTALL_DIR)/lib/$(STATIC_NAME); \
	    if [ -f crt0.o ] ; then \
		rm -f $(INSTALL_DIR)/lib/crt0.o; \
		cp crt0.o $(INSTALL_DIR)/lib ; \
		chmod 644 $(INSTALL_DIR)/lib/crt0.o; \
		chown -R root.root $(INSTALL_DIR)/lib/crt0.o; \
	    fi; \
	    chmod -R 775 `find $(INSTALL_DIR)/include -type d`; \
	    chmod -R 644 `find $(INSTALL_DIR)/include -type f`; \
	    chown -R root.root $(INSTALL_DIR)/include; \
	fi;

.PHONY: dummy

