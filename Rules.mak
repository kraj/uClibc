# Rules.make for uClibc
#
# This file contains rules which are shared between multiple Makefiles.  All
# normal configuration options live in the file named "Config".  You probably
# should not mess with this file unless you know what you are doing...  
#   -Erik Andersen <andersen@lineo.com> < andersee@debian.org>
# 
# Copyright (C) 2000, 2001 by Lineo, inc.
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

include $(TOPDIR)Config

GCCINCDIR = $(shell $(CC) -print-search-dirs | sed -ne "s/install: \(.*\)/\1include/gp")

# use '-Os' optimization if available, else use -O2, allow Config to override
ifndef OPTIMIZATION
OPTIMIZATION = $(shell if $(CC) -Os -S -o /dev/null -xc /dev/null >/dev/null 2>&1; \
    then echo "-Os"; else echo "-O2" ; fi)
endif

ARFLAGS=r

CCFLAGS=$(WARNINGS) $(OPTIMIZATION) -fno-builtin -nostdinc $(CPUFLAGS) -I$(TOPDIR)include -I$(GCCINCDIR) -I. -D_LIBC

CFLAGS=$(ARCH_CFLAGS) $(CCFLAGS) $(DEFS) $(ARCH_CFLAGS2)

ifeq ($(strip $(DODEBUG)),true)
    CFLAGS += -g
    LDFLAGS = -nostdlib -Wl,-warn-common 
    STRIPTOOL = /bin/true -Since_we_are_debugging
else
    CFLAGS  += -DNDEBUG #-fomit-frame-pointer
    LDFLAGS  = -s -nostdlib -Wl,-warn-common
endif

ifndef $(PREFIX)
    PREFIX = `pwd`/_install
endif

NATIVE_ARCH = $(shell uname -m | sed -e 's/i.86/i386/' -e 's/sparc.*/sparc/' -e 's/arm.*/arm/g' -e 's/m68k.*/m68k/')

# It turns out the currently, function-sections causes ldelf2flt to segfault.
# So till further notice, this is disabled by default....
# 
# Use '-ffunction-sections -fdata-sections' and '--gc-sections' if they work
# to try and strip out any unused junk automagically....
#
#ifeq ($(shell $(CC) -ffunction-sections -fdata-sections -S \
#	-o /dev/null -xc /dev/null && $(LD) --gc-sections -v >/dev/null && echo 1),1)
#    CFLAGS += -ffunction-sections -fdata-sections
#    LDFLAGS += --gc-sections
#endif


