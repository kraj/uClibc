# Makefile for uClibc
#
# Copyright (C) 2000-2003 Erik Andersen <andersen@uclibc.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General
# Public License along with this program; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place, Suite 330,
# Boston, MA 02111-1307 USA


#--------------------------------------------------------------
# You shouldn't need to mess with anything beyond this point...
#--------------------------------------------------------------
noconfig_targets := menuconfig config oldconfig randconfig \
	defconfig allyesconfig allnoconfig clean distclean \
	release tags TAGS
TOPDIR=./
include Rules.mak

DIRS = extra ldso libc libcrypt libresolv libnsl libutil libm libpthread
ifeq ($(strip $(UCLIBC_HAS_GETTEXT_AWARENESS)),y)
	DIRS += libintl
endif

ifeq ($(strip $(HAVE_DOT_CONFIG)),y)

all: headers pregen subdirs shared utils finished

# In this section, we need .config
-include .config.cmd

.PHONY: $(SHARED_TARGET)
shared: $(SHARED_TARGET)
ifeq ($(strip $(HAVE_SHARED)),y)
	@$(MAKE) -C libc shared
	@$(MAKE) -C ldso shared
	@$(MAKE) -C libcrypt shared
	@$(MAKE) -C libresolv shared
	@$(MAKE) -C libnsl shared
	@$(MAKE) -C libutil shared
	@$(MAKE) -C libm shared
	@$(MAKE) -C libpthread shared
ifeq ($(strip $(UCLIBC_HAS_GETTEXT_AWARENESS)),y)
	@$(MAKE) -C libintl shared
endif
else
ifeq ($(SHARED_TARGET),)
	@echo
	@echo Not building shared libraries...
	@echo
endif
endif

ifneq ($(SHARED_TARGET),)

lib/main.o: $(ROOTDIR)/lib/libc/main.c
	$(CC) $(CFLAGS) $(ARCH_CFLAGS) -c -o $@ $(ROOTDIR)/lib/libc/main.c

bogus $(SHARED_TARGET): lib/libc.a lib/main.o Makefile
	make -C $(ROOTDIR) relink
	$(CC) -nostartfiles -o $(SHARED_TARGET) $(ARCH_CFLAGS) -Wl,-elf2flt -nostdlib		\
		-Wl,-shared-lib-id,${LIBID}				\
		lib/main.o \
		-Wl,--whole-archive,lib/libc.a,-lgcc,--no-whole-archive
	$(OBJCOPY) -L _GLOBAL_OFFSET_TABLE_ -L main -L __main -L _start \
		-L __uClibc_main -L __uClibc_start_main -L lib_main \
		-L _exit_dummy_ref		\
		-L __do_global_dtors -L __do_global_ctors		\
		-L __CTOR_LIST__ -L __DTOR_LIST__			\
		-L _current_shared_library_a5_offset_			\
		$(SHARED_TARGET).gdb
	$(LN) -sf $(SHARED_TARGET).gdb .
endif

finished: shared
	@echo
	@echo Finally finished compiling...
	@echo

#
# Target for uClinux distro
#
.PHONY: romfs
romfs:
	@if [ "$(CONFIG_BINFMT_SHARED_FLAT)" = "y" ]; then \
		[ -e $(ROMFSDIR)/lib ] || $(INSTALL) -d $(ROMFSDIR)/lib; \
		$(ROMFSINST) $(SHARED_TARGET) /lib/lib$(LIBID).so; \
	fi
ifeq ($(strip $(HAVE_SHARED)),y)
	$(INSTALL) -d $(ROMFSDIR)/lib
	$(INSTALL) -m 644 lib/lib*-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
		$(ROMFSDIR)/lib
	cp -fa lib/*.so.* $(ROMFSDIR)/lib/.
	@if [ -x lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so ] ; then \
	    set -x -e; \
	    $(INSTALL) -m 755 lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
	    		$(ROMFSDIR)/lib; \
		$(ROMFSINST) -s \
			/lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
			/lib/ld-linux.so.2; \
	fi;
endif

include/bits/uClibc_config.h: .config
	@if [ ! -x ./extra/config/conf ] ; then \
	    make -C extra/config conf; \
	fi;
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -o extra/Configs/Config.$(TARGET_ARCH)

headers: include/bits/uClibc_config.h
ifeq ($(strip $(UCLIBC_HAS_MMU)),y)
	@./extra/scripts/fix_includes.sh -k $(KERNEL_SOURCE) -t $(TARGET_ARCH)
else
	@./extra/scripts/fix_includes.sh -k $(KERNEL_SOURCE) -t $(TARGET_ARCH) -n
endif
	@if [ "$(TARGET_ARCH)" = "mipsel" ]; then \
	    $(LN) -fs mips libc/sysdeps/linux/mipsel; \
	    $(LN) -fs mips ldso/ldso/mipsel; \
	    $(LN) -fs mips libpthread/linuxthreads/sysdeps/mipsel; \
	fi;
	@cd include/bits; \
	set -e; \
	for i in `ls ../../libc/sysdeps/linux/common/bits/*.h` ; do \
		$(LN) -fs $$i .; \
	done; \
	if [ -d ../../libc/sysdeps/linux/$(TARGET_ARCH)/bits ] ; then \
		for i in `ls ../../libc/sysdeps/linux/$(TARGET_ARCH)/bits/*.h` ; do \
			$(LN) -fs $$i .; \
		done; \
	fi
	@cd include/sys; \
	set -e; \
	for i in `ls ../../libc/sysdeps/linux/common/sys/*.h` ; do \
		$(LN) -fs $$i .; \
	done; \
	if [ -d ../../libc/sysdeps/linux/$(TARGET_ARCH)/sys ] ; then \
		for i in `ls ../../libc/sysdeps/linux/$(TARGET_ARCH)/sys/*.h` ; do \
			$(LN) -fs $$i .; \
		done; \
	fi
	@cd $(TOPDIR); \
	set -x -e; \
	TOPDIR=. CC="$(CC)" /bin/sh extra/scripts/gen_bits_syscall_h.sh > include/bits/sysnum.h.new; \
	if cmp include/bits/sysnum.h include/bits/sysnum.h.new >/dev/null 2>&1; then \
		$(RM) include/bits/sysnum.h.new; \
	else \
		mv -f include/bits/sysnum.h.new include/bits/sysnum.h; \
	fi
	$(MAKE) -C libc/sysdeps/linux/$(TARGET_ARCH) headers

# Command used to download source code
WGET:=wget --passive-ftp

pregen: headers
ifeq ($(strip $(UCLIBC_DOWNLOAD_PREGENERATED_LOCALE_DATA)),y)
	(cd extra/locale; \
	$(WGET) http://www.uclibc.org/downloads/uClibc-locale-030818.tgz);
endif
ifeq ($(strip $(UCLIBC_PREGENERATED_LOCALE_DATA)),y)
	(cd extra/locale; zcat uClibc-locale-030818.tgz | tar -xvf -)
	make -C extra/locale pregen
endif


subdirs: $(patsubst %, _dir_%, $(DIRS))

$(patsubst %, _dir_%, $(DIRS)) : dummy
	$(MAKE) -C $(patsubst _dir_%, %, $@)

tags:
	ctags -R

install: install_dev install_runtime install_toolchain install_utils finished2


# Installs header files and development library links.
install_dev:
	$(INSTALL) -d $(PREFIX)$(DEVEL_PREFIX)/lib
	$(INSTALL) -d $(PREFIX)$(DEVEL_PREFIX)/usr/lib
	$(INSTALL) -d $(PREFIX)$(DEVEL_PREFIX)/include
	-$(INSTALL) -m 644 lib/*.[ao] $(PREFIX)$(DEVEL_PREFIX)/lib/
	tar -chf - include | tar -xf - -C $(PREFIX)$(DEVEL_PREFIX);
ifneq ($(strip $(UCLIBC_HAS_FLOATS)),y)
	# Remove floating point related headers since float support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/complex.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/fpu_control.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/ieee754.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/math.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/tgmath.h
endif
ifneq ($(strip $(UCLIBC_HAS_WCHAR)),y)
	# Remove wide char headers since wide char support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/wctype.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/wchar.h
endif
ifneq ($(strip $(UCLIBC_HAS_LOCALE)),y)
	# Remove iconv header since locale support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/iconv.h
endif
ifneq ($(strip $(UCLIBC_HAS_GLIBC_CUSTOM_PRINTF)),y)
	# Remove printf header since custom print specifier support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/printf.h
endif
ifneq ($(strip $(UCLIBC_HAS_XLOCALE)),y)
	# Remove xlocale header since extended locale support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/xlocale.h
endif
ifneq ($(strip $(UCLIBC_HAS_GETTEXT_AWARENESS)),y)
	# Remove libintl header since gettext support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/libintl.h
endif
ifneq ($(strip $(UCLIBC_HAS_REGEX)),y)
	# Remove regex headers since regex support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/regex.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/regexp.h
endif
ifneq ($(strip $(UCLIBC_HAS_WORDEXP)),y)
	# Remove wordexp header since wordexp support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/wordexp.h
endif
ifneq ($(strip $(UCLIBC_HAS_FTW)),y)
	# Remove ftw header since ftw support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/ftw.h
endif
ifneq ($(strip $(UCLIBC_HAS_GLOB)),y)
	# Remove glob header since glob support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/glob.h
endif
ifneq ($(strip $(UCLIBC_HAS_GNU_GETOPT)),y)
	# Remove getopt header since gnu getopt support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)/include/getopt.h
endif
	-@for i in `find  $(PREFIX)$(DEVEL_PREFIX) -type d` ; do \
	    chmod -f 755 $$i; chmod -f 644 $$i/*.h; \
	done;
	-find $(PREFIX)$(DEVEL_PREFIX) -name CVS | xargs $(RM) -r;
	-chown -R `id | sed 's/^uid=\([0-9]*\).*gid=\([0-9]*\).*$$/\1.\2/'` $(PREFIX)$(DEVEL_PREFIX)
ifeq ($(strip $(HAVE_SHARED)),y)
	-$(INSTALL) -m 644 lib/*.so $(PREFIX)$(DEVEL_PREFIX)/lib/
	-find lib/ -type l -name '*.so' -exec cp -fa {} $(PREFIX)$(DEVEL_PREFIX)/lib ';'
	# If we build shared libraries then the static libs are PIC...
	# Make _pic.a symlinks to make mklibs.py and similar tools happy.
	for i in `find lib/  -type f -name '*.a' | sed -e 's/lib\///'` ; do \
		$(LN) -sf $$i $(PREFIX)$(DEVEL_PREFIX)/lib/`echo $$i | sed -e 's/\.a$$/_pic.a/'`; \
	done
endif


# Installs run-time libraries and helper apps onto the host system
# allowing cross development.  If you want to deploy to a target 
# system, use the "install_target" target instead... 
install_runtime:
ifeq ($(strip $(HAVE_SHARED)),y)
	$(INSTALL) -d $(PREFIX)$(RUNTIME_PREFIX)/lib
	$(INSTALL) -d $(PREFIX)$(RUNTIME_PREFIX)/bin
	$(INSTALL) -m 644 lib/lib*-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
		$(PREFIX)$(RUNTIME_PREFIX)/lib
	cp -fa lib/*.so.* $(PREFIX)$(RUNTIME_PREFIX)/lib
	@if [ -x lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so ] ; then \
	    set -x -e; \
	    $(INSTALL) -m 755 lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
	    		$(PREFIX)$(RUNTIME_PREFIX)/lib; \
	fi;
endif

install_toolchain:
	$(INSTALL) -d $(PREFIX)$(DEVEL_PREFIX)/lib
	$(INSTALL) -d $(PREFIX)$(DEVEL_PREFIX)/bin
	$(INSTALL) -d $(PREFIX)$(DEVEL_TOOL_PREFIX)/bin
	$(INSTALL) -d $(PREFIX)$(SYSTEM_DEVEL_PREFIX)/bin
	$(MAKE) -C extra/gcc-uClibc install

ifeq ($(strip $(HAVE_SHARED)),y)
utils: $(TOPDIR)ldso/util/ldd
	$(MAKE) -C ldso utils
else
utils: dummy
endif

install_utils: utils
ifeq ($(strip $(HAVE_SHARED)),y)
	$(INSTALL) -d $(PREFIX)$(DEVEL_TOOL_PREFIX)/bin;
	$(INSTALL) -m 755 ldso/util/ldd \
		$(PREFIX)$(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-ldd
	$(LN) -fs $(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-ldd \
		$(PREFIX)$(DEVEL_TOOL_PREFIX)/bin/ldd
	# For now, don't bother with readelf since surely the host
	# system has binutils, or we couldn't have gotten this far...
	#$(INSTALL) -m 755 ldso/util/readelf \
	#	$(PREFIX)$(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-readelf
	#$(LN) -fs $(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-readelf \
	#	$(PREFIX)$(DEVEL_TOOL_PREFIX)/bin/readelf
	@if [ -x ldso/util/ldconfig ] ; then \
	    set -x -e; \
	    $(INSTALL) -d $(PREFIX)$(DEVEL_PREFIX)/etc; \
	    $(INSTALL) -m 755 ldso/util/ldconfig \
		    $(PREFIX)$(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-ldconfig; \
	    $(LN) -fs $(SYSTEM_DEVEL_PREFIX)/bin/$(TARGET_ARCH)-uclibc-ldconfig \
		    $(PREFIX)$(DEVEL_TOOL_PREFIX)/bin/ldconfig; \
	fi;
endif

# Installs run-time libraries and helper apps in preparation for
# deploying onto a target system, but installed below wherever
# $PREFIX is set to, allowing you to package up the result for
# deployment onto your target system.
install_target:
ifeq ($(strip $(HAVE_SHARED)),y)
	$(INSTALL) -d $(PREFIX)$(TARGET_PREFIX)/lib
	$(INSTALL) -d $(PREFIX)$(TARGET_PREFIX)/usr/bin
	$(INSTALL) -m 644 lib/lib*-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
		$(PREFIX)$(TARGET_PREFIX)/lib
	cp -fa lib/*.so.* $(PREFIX)$(TARGET_PREFIX)/lib
	@if [ -x lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so ] ; then \
	    set -x -e; \
	    $(INSTALL) -m 755 lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
	    		$(PREFIX)$(TARGET_PREFIX)/lib; \
	fi;
endif

install_target_utils:
ifeq ($(strip $(HAVE_SHARED)),y)
	@$(MAKE) -C ldso/util ldd.target ldconfig.target #readelf.target
	$(INSTALL) -d $(PREFIX)$(TARGET_PREFIX)/etc;
	$(INSTALL) -d $(PREFIX)$(TARGET_PREFIX)/sbin;
	$(INSTALL) -d $(PREFIX)$(TARGET_PREFIX)/usr/bin;
	$(INSTALL) -m 755 ldso/util/ldd.target $(PREFIX)$(TARGET_PREFIX)/usr/bin/ldd
	$(INSTALL) -m 755 ldso/util/ldconfig.target $(PREFIX)$(TARGET_PREFIX)/sbin/ldconfig;
	#$(INSTALL) -m 755 ldso/util/readelf.target $(PREFIX)$(TARGET_PREFIX)/usr/bin/readelf;
endif
ifeq ($(strip $(UCLIBC_HAS_LOCALE)),y)
	@$(MAKE) -C libc/misc/wchar iconv.target
	$(INSTALL) -d $(PREFIX)$(TARGET_PREFIX)/usr/bin;
	$(INSTALL) -m 755 libc/misc/wchar/iconv.target $(PREFIX)$(TARGET_PREFIX)/usr/bin/iconv
endif

finished2:
	@echo
	@echo Finished installing...
	@echo

else # ifeq ($(strip $(HAVE_DOT_CONFIG)),y)

all: menuconfig

# configuration
# ---------------------------------------------------------------------------
extra/config/conf: buildconf
	-@if [ ! -f .config ] ; then \
		cp extra/Configs/Config.$(TARGET_ARCH).default .config; \
	fi

buildconf:
	make -C extra/config conf

extra/config/mconf: buildmconf
	-@if [ ! -f .config ] ; then \
		cp extra/Configs/Config.$(TARGET_ARCH).default .config; \
	fi

buildmconf:
	make -C extra/config ncurses mconf

menuconfig: extra/config/mconf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/mconf extra/Configs/Config.$(TARGET_ARCH)

config: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf extra/Configs/Config.$(TARGET_ARCH)

oldconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -o extra/Configs/Config.$(TARGET_ARCH)

randconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -r extra/Configs/Config.$(TARGET_ARCH)

allyesconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -y extra/Configs/Config.$(TARGET_ARCH)

allnoconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -n extra/Configs/Config.$(TARGET_ARCH)

defconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -d extra/Configs/Config.$(TARGET_ARCH)


clean:
	- find . \( -name \*.o -o -name \*.a -o -name \*.so -o -name core -o -name .\#\* \) -exec $(RM) {} \;
	@$(RM) -r tmp lib include/bits libc/tmp _install
	$(MAKE) -C test clean
	$(MAKE) -C ldso clean
	$(MAKE) -C libc/misc/internals clean
	$(MAKE) -C libc/misc/wchar clean
	$(MAKE) -C libc/unistd clean
	$(MAKE) -C libc/sysdeps/linux/common clean
	$(MAKE) -C extra/gcc-uClibc clean
	$(MAKE) -C extra/locale clean
	@set -e; \
	for i in `(cd $(TOPDIR)/libc/sysdeps/linux/common/sys; ls *.h)` ; do \
		$(RM) include/sys/$$i; \
	done; \
	if [ -d libc/sysdeps/linux/$(TARGET_ARCH)/sys ] ; then \
		for i in `(cd libc/sysdeps/linux/$(TARGET_ARCH)/sys; ls *.h)` ; do \
			$(RM) include/sys/$$i; \
		done; \
	fi;
	@$(RM) include/linux include/scsi include/asm
	@if [ -d libc/sysdeps/linux/$(TARGET_ARCH) ]; then		\
	    $(MAKE) -C libc/sysdeps/linux/$(TARGET_ARCH) clean;		\
	fi;
	@if [ "$(TARGET_ARCH)" = "mipsel" ]; then \
	    $(MAKE) -C libc/sysdeps/linux/mips clean; \
	    $(RM) ldso/ldso/mipsel; \
	    $(RM) libc/sysdeps/linux/mipsel; \
	    $(RM) libpthread/linuxthreads/sysdeps/mipsel; \
	fi;

distclean: clean
	$(RM) .config .config.old .config.cmd
	$(MAKE) -C extra clean

release: distclean
	cd ..;					\
	$(RM) -r uClibc-$(VERSION);		\
	cp -fa uClibc uClibc-$(VERSION);	\
	find uClibc-$(VERSION)/ -type f		\
	    -name .\#* -exec $(RM) -r {} \; ;	\
	find uClibc-$(VERSION)/ -type d		\
	    -name CVS  -exec $(RM) -r {} \; ;	\
						\
	tar -cvzf uClibc-$(VERSION).tar.gz uClibc-$(VERSION)/;

endif # ifeq ($(strip $(HAVE_DOT_CONFIG)),y)

.PHONY: dummy subdirs release distclean clean config oldconfig menuconfig


