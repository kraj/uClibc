# Makefile for uCLibc
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
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA 02111-1307 USA
#
# Derived in part from the Linux-8086 C library, the GNU C Library, and several
# other sundry sources.  Files within this library are copyright by their
# respective copyright holders.

#MALLOC = malloc
MALLOC = malloc-simple

DIRS = error getent $(MALLOC) misc regex stdio \
	    string termios time sysdeps shm #rpc
all: libc.a

libc.a: halfclean headers subdirs
	@echo
	@echo Finally finished compiling...
	@echo
	$(CROSS)ranlib libc.a

halfclean:
	@rm -f libc.a

headers: dummy
	@if [ ! -L "include/asm" ]; then ln -s /usr/include/asm include/asm ; fi
	@if [ ! -L "include/net" ]; then ln -s /usr/include/net include/net ; fi
	@if [ ! -L "include/linux" ]; then ln -s /usr/include/linux include/linux ; fi

tags:
	ctags -R
	
clean: subdirs_clean
	rm -f libc.a

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(DIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

.PHONY: dummy

