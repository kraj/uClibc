# Library Configuration rules for uClibc
#
# This file contains rules which are shared between multiple Makefiles.  All
# normal configuration options live in the file named "Config".  You probably
# should not mess with this file unless you know what you are doing...  
# 
# Copyright (C) 2000 by Lineo, inc.
# Copyright (C) 2001 by Hewlett-Packard Australia
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

NATIVE_CC = gcc

# If you are running a cross compiler, you may want to set this
# to something more interesting...  Target architecture is determined
# by asking this compiler what arch it compiles stuff for.
CROSS = /usr/cygnus/yapp-001013/H-i686-pc-linux-gnulibc2.1/bin/sh-linux-gnu-
CC = $(CROSS)gcc
AR = $(CROSS)ar
LD = $(CROSS)ld
NM = $(CROSS)nm
STRIPTOOL = $(CROSS)strip
#STRIPTOOL = /bin/true

# Set the following to `true' to make a debuggable build, and `false' for
# production builds.
DODEBUG = false

# Compiler warnings you want to see 
WARNINGS=-Wall

# Note that the kernel source you use to compile with should be the same as the
# Linux kernel you run your apps on.  uClibc doesn't even try to achieve binary
# compatibility across kernel versions.  So don't expect, for example, uClibc
# compiled with Linux kernel 2.0.x to implement lchown properly, since 2.0.x
# can't do that. Similarly, if you compile uClibc vs Linux 2.4.x kernel headers,
# but then run on Linux 2.0.x, lchown will be compiled into uClibc, but won't
# work at all.  You have been warned.
KERNEL_SOURCE=../../../linux

#
# ARCH_CFLAGS if your have something special to add to the CFLAGS
#
ARCH_CFLAGS  = -DNO_UNDERSCORES

ifeq ($(strip $(TARGET_ARCH)),sh)
ifeq ($(strip $(TARGET_PROC)),SH2_BIG_THAMIS)
ARCH_CFLAGS += -DHIOS -mb -m2 -specs=/usr/cygnus/yapp-001013/H-i686-pc-linux-gnulibc2.1/lib/gcc-lib/sh-linux-gnu/2.96-yapp-001013/specs_sh2
HAS_MMU = false
endif
ifeq ($(strip $(TARGET_PROC)),SH3_BIG_UCLINUX)
ARCH_CFLAGS += -mb
HAS_MMU = false
endif
ifeq ($(strip $(TARGET_PROC)),SH3_LITTLE_UCLINUX)
ARCH_CFLAGS += -ml
HAS_MMU = false
endif
ifeq ($(strip $(TARGET_PROC)),SH3_BIG)
ARCH_CFLAGS += -mb -m3 -D__BIG_ENDIAN__
HAS_MMU = true
endif
ifeq ($(strip $(TARGET_PROC)),SH3)
ARCH_CFLAGS += -ml
HAS_MMU = true
endif
ifeq ($(strip $(TARGET_PROC)),SH4)
ARCH_CFLAGS += -ml -m4
HAS_MMU = true
endif
endif

# Set this to `false' if you don't have/need basic floating point support
# support in libc (strtod, printf, scanf).  Set it to `true' otherwise.
# If this is not true, then libm will not be built.
HAS_FLOATING_POINT = true

# Set to `true' if you want the math library to contain the full set
# of C99 math library features.  Costs an extra 35k or so on x86. 
DO_C99_MATH = false

# Set this to 'false if you don't need shadow password support.
HAS_SHADOW = false

# Set this to `false' if you don't have/need locale support; `true' otherwise.
# NOTE: Currently does not affect collation.
# You must also generate the locale data and associated .h file.
# See the README in directory extra/locale for details.
HAS_LOCALE = false

# Set this to `false' if you don't have/need wide char support.
HAS_WCHAR = false

# This specifies which malloc implementation is used.
#
# "malloc" use mmap for all allocations and so works very well on MMU-less
# systems that do not support the brk() system call.   It is pretty smart 
# about reusing already allocated memory, and minimizing memory wastage.
#
# "malloc-930716" is derived from libc-5.3.12 and uses the brk() system call
# for all memory allocations.  This makes it very fast.  It is also pretty
# smart about reusing already allocated memory, and minimizing memory wastage.
# Because this uses brk() it will not work on uClinux MMU-less systems.
MALLOC = malloc 
#MALLOC = malloc-930716

# If you want large file support (greater then 2 GiB) turn this on.
# Do not enable this unless your kernel provides large file support.
DOLFS = false

# Posix regular expression code is really big -- 27k all by itself.
# If you don't use regular expressions, turn this off and save space.
# Of course, if you only staticly link, leave this on, since it will
# only be included in your apps if you use regular expressions. 
INCLUDE_REGEX=true

# If you want to include RPC support, enable this.  RPC is almost never used 
# for anything except NFS support, so unless you plan to use NFS, leave this
# disabled.  This is off by default.
INCLUDE_RPC = false

# Normally we enable just enough RPC support for things like rshd and 
# nfs mount to work.  If you find you need the rest of the RPC stuff, 
# then enable this.
INCLUDE_FULL_RPC = false

# If you want to include support for the next version of the Internet
# Protocol: IP version 6, enable this.  This is off by default.
INCLUDE_IPV6 = false

# If you want to support only Unix 98 PTYs enable this.  Some older
# applications may need this disabled.  For most current programs, 
# you can generally leave this true.
UNIX98PTY_ONLY = true

# Enable this if /dev/pts is on a devpts or devfs file system.  Both
# these filesystems automatically manage permissions on the /dev/pts 
# devices.  You may need to mount this fs on /dev/pts for this to work. 
# This is true by default.
ASSUME_DEVPTS = true


# If you want to compile the library as PIC code, turn this on.
# This is automagically enabled when HAVE_SHARED is true
DOPIC = false

# Enable support for shared libraries?  If this is false, you can
# ignore all the rest of the options in this file...
HAVE_SHARED = false

# uClibc has a native shared library loader for some architectures.
BUILD_UCLIBC_LDSO=false

# If you are using shared libraries, but do not want/have a native
# uClibc shared library loader, please specify the name of your
# system's shared library loader here...
#SYSTEM_LDSO=/lib/ld-linux.so.2

# When using shared libraries, this path is the location where the
# shared library will be invoked.  This value will be compiled into
# every binary compiled with uClibc.  
#
# BIG FAT WARNING:  
# If you do not have a shared library loader with the correct name
# sitting in the directory this points to, your binaries will not run.
SHARED_LIB_LOADER_PATH=$(DEVEL_PREFIX)/lib

# DEVEL_PREFIX is the directory into which the uClibc development
# environment will be installed.   The result will look something 
# like the following:
#   DEVEL_PREFIX/
#	lib/            <contains all runtime and static libs>
#	include/        <Where all the header files go>
# This value is used by the 'make install' Makefile target.  Since this
# directory is compiled into the uclibc cross compiler spoofer, you
# have to recompile if you change this value...
DEVEL_PREFIX = /usr/$(TARGET_ARCH)-linux-uclibc

# SYSTEM_DEVEL_PREFIX is the directory prefix used when installing
# bin/arch-uclibc-gcc, bin/arch-uclibc-ld, etc.   This is only used by
# the 'make install' target, and is not compiled into anything.  This
# defaults to $DEVEL_PREFIX/usr, but makers of .rpms and .debs will
# want to set this to "/usr" instead.
SYSTEM_DEVEL_PREFIX = $(DEVEL_PREFIX)

# DEVEL_TOOL_PREFIX is the directory prefix used when installing
# bin/gcc, bin/ld, etc.   This is only used by the 'make install'
# target, and is not compiled into anything.  This defaults to
# $DEVEL_PREFIX/usr, but makers of .rpms and .debs may want to
# set this to something else.
DEVEL_TOOL_PREFIX = $(DEVEL_PREFIX)/usr

# If you want 'make install' to install everything under a temporary
# directory, the define PREFIX during the install step,
# i.e., 'make PREFIX=/var/tmp/uClibc install'.
#PREFIX = $(TOPDIR)/_install

#Enable .hidden asm directives
HAVE_DOT_HIDDEN=true
