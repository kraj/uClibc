# Rules.make for uCLibc
#
# This file contains rules which are shared between multiple Makefiles.
# Feel free to adjust to taste...
#  -Erik Andersen <andersen@lineo.com> < andersee@debian.org>
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

PROG      := libc.a
VERSION   := 0.95
BUILDTIME := $(shell TZ=UTC date --utc "+%Y.%m.%d-%H:%M%z")
export VERSION

# Set the following to `true' to make a debuggable build.
# Do not enable this for production builds...
DODEBUG = false

# This specifies which malloc implementation is used.
# "malloc-simple" is very, very small, but is also very, very dumb 
# and does not try to make good use of memory or clean up after itself.
# "malloc" on the other hand is a bit bigger, but is pretty smart thereby
# minimizing memory wastage and reusing already allocated memory.  This 
# can be lots faster and safer IMHO.
#MALLOC = malloc-simple
MALLOC = malloc 

# If you want large file summit support (greater then 2 Gib), 
# turn this on.  This has no effect unless your kernel supports 
# lfs.  This surrently does nothing...
DOLFS = false

# Enable stuff that is broken (to fix it of course....)
DO_FIXME_STUFF = true

# Disable this if your CPU has a memory management unit (MMU)
HAS_MMU = true

# Disable this if your CPU has a floating point unit (FPU)
HAS_FLOATS = true

# If you are running a cross compiler, you may want to set this
# to something more interesting...
CROSS = #powerpc-linux-
CC = $(CROSS)gcc
STRIPTOOL = $(CROSS)strip


#--------------------------------------------------------
# Nothing beyond this point should need be touched by mere 
# mortals so you should probably leave this stuff alone.
#--------------------------------------------------------

GCCINCDIR = $(shell gcc -print-search-dirs | sed -ne "s/install: \(.*\)/\1include/gp")

ARFLAGS=r

CCFLAGS= $(OPTIMIZATION) -fno-builtin -nostdinc $(CPUFLAGS) -Dlinux -D__linux__ -I$(TOPDIR)include -I$(GCCINCDIR) -I. -D__PIC__ -D__LIBC__
CFLAGS=$(ARCH) $(CCFLAGS) $(DEFS)

ifeq ($(DODEBUG),true)
    CFLAGS += -Wall -g -D__PIC__
    LDFLAGS = -nostdlib 
else
    CFLAGS  += -Wall  -D__PIC__ #-fomit-frame-pointer
    LDFLAGS  = -s -nostdlib
endif

ifndef $(PREFIX)
    PREFIX = `pwd`/_install
endif

ifneq ($(HAS_MMU),true)
    CFLAGS += -D__HAS_NO_MMU__
endif

ifneq ($(HAS_FLOATS),true)
    CFLAGS += -D__HAS_NO_FLOATS__
endif

ifneq ($(DO_FIXME_STUFF),true)
    CFLAGS += -DFIXME
endif



# Use '-ffunction-sections -fdata-sections' and '--gc-sections' if they work
# to try and strip out any unused junk automagically....
#
ifeq ($(shell $(CC) -ffunction-sections -fdata-sections -S \
	-o /dev/null -xc /dev/null && $(LD) --gc-sections -v >/dev/null && echo 1),1)
    CFLAGS += -ffunction-sections -fdata-sections
    LDFLAGS += --gc-sections
endif


