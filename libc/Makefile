# Makefile for uClibc
#
# Copyright (C) 2000 by Lineo, inc.
# Copyright (C) 2000,2001 Erik Andersen <andersen@uclibc.org>
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

# Check if the target architecture has a version script for
# libc, and if so, include it when linking.
VERSION_SCRIPT:=${shell if [ -f sysdeps/linux/$(TARGET_ARCH)/libc.map ] ; then \
        echo "--version-script sysdeps/linux/$(TARGET_ARCH)/libc.map"; fi}

# we have SHARED_MAJORNAME=libc.so.$(MAJOR_VERSION) defined in Rules.mak
LIB_NAME=libc
AR_LIB_NAME=$(TOPDIR)lib/$(LIB_NAME).a
SO_LIB_NAME=$(TOPDIR)lib/$(LIB_NAME).so
SO_FULL_NAME=libuClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

ifeq ($(strip $(HAVE_SHARED)),y)
all: $(SO_LIB_NAME)
else
all: $(AR_LIB_NAME)
endif

# Some functions are duplicated across subdirs, and when you pass $(AR)
# the same object file more than once, it'll add it to the archive multiple 
# times (boo!).  So what we do here is:
#  - import all the objects (thus we may have dupes)
#  - delete all the dupes
#  - re-import certain objects based upon preference
#    - the sysdeps dir should override all other symbols for example
# We need to use shell globbing with obj.* because if we use make's wildcard,
# the wildcard will be evaluated when `make` is run instead of when the make 
# target is evaluated.  That means if you run `rm obj.* ; make`, the wildcard 
# will evaluate to no files :(.
shared_$(LIB_NAME).a: subdirs
	$(RM) shared_$(LIB_NAME).a
	objs=`cat obj.*` ; \
	$(AR) $(ARFLAGS) shared_$(LIB_NAME).a $$objs && \
	$(AR) dN 2 shared_$(LIB_NAME).a $$objs && \
	$(AR) dN 2 shared_$(LIB_NAME).a $$objs
	@for objfile in obj.signal \
	                obj.string.generic obj.string.$(TARGET_ARCH) obj.string \
	                obj.sysdeps.common obj.sysdeps.$(TARGET_ARCH) ; do \
		if [ -e $$objfile ] ; then \
			if [ "$(MAKE_IS_SILENT)" = "n" ] ; then \
				echo $(AR) $(ARFLAGS) shared_$(LIB_NAME).a $$objfile ; \
			fi ; \
			objs=`cat $$objfile` ; \
		fi ; \
		$(AR) $(ARFLAGS) shared_$(LIB_NAME).a $$objs || exit 1 ; \
	done

$(AR_LIB_NAME): shared_$(LIB_NAME).a
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(AR_LIB_NAME)
	cp shared_$(LIB_NAME).a $(AR_LIB_NAME)
	$(AR) $(ARFLAGS) $(AR_LIB_NAME) misc/internals/static.o `cat nonshared_obj.*`

$(SO_LIB_NAME): shared_$(LIB_NAME).a
	$(RM) $(TOPDIR)lib/$(SO_FULL_NAME) $(TOPDIR)lib/$(SHARED_MAJORNAME) $(SO_LIB_NAME)
	$(LD) $(LDFLAGS) $(VERSION_SCRIPT) -soname=$(SHARED_MAJORNAME) \
		-o $(TOPDIR)lib/$(SO_FULL_NAME) --whole-archive shared_$(LIB_NAME).a \
		--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
		-init __uClibc_init $(TOPDIR)lib/$(UCLIBC_LDSO) $(LDADD_LIBFLOAT) $(LIBGCC)
	$(LN) -sf $(SO_FULL_NAME) $(TOPDIR)lib/$(SHARED_MAJORNAME)
	$(RM) $(TOPDIR)lib/$(NONSHARED_LIBNAME)
	$(AR) $(ARFLAGS) $(TOPDIR)lib/$(NONSHARED_LIBNAME) `cat nonshared_obj.*`
	echo "/* GNU ld script" > $(SO_LIB_NAME)
	echo " * Use the shared library, but some functions are only in" >> $(SO_LIB_NAME)
	echo " * the static library, so try that secondarily. */" >> $(SO_LIB_NAME)
	#OUT_FORMAT:=$(shell $(LD) --verbose | grep OUTPUT_FORMAT | awk -F '"' '{print $2}')
	#echo "OUTPUT_FORMAT($(OUT_FORMAT))" >> $(SO_LIB_NAME)
ifeq ($(strip $(COMPAT_ATEXIT)),y)
	echo "GROUP ( $(TOPDIR)lib/$(NONSHARED_LIBNAME) $(TOPDIR)lib/$(SHARED_MAJORNAME) )" >> $(SO_LIB_NAME)
else
	echo "GROUP ( $(TOPDIR)lib/$(SHARED_MAJORNAME) $(TOPDIR)lib/$(NONSHARED_LIBNAME) )" >> $(SO_LIB_NAME)
endif

tags:
	ctags -R

clean: subdirs_clean
	$(RM) *.a obj.* nonshared_obj.*

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(DIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

.PHONY: dummy subdirs
