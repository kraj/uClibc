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
	release tags
TOPDIR=./
include Rules.mak

RTLD_DIR =
ifeq ($(HAVE_SHARED),y)
RTLD_DIR = ldso
endif
# need to have libc built, before we can build the others
PRE_DIRS = $(RTLD_DIR) libc
DIRS = $(RTLD_DIR) libcrypt libresolv libnsl libutil librt
ifeq ($(UCLIBC_HAS_FLOATS),y)
	DIRS += libm
endif
ifeq ($(UCLIBC_HAS_THREADS),y)
	DIRS += libpthread
endif
ifeq ($(UCLIBC_HAS_GETTEXT_AWARENESS),y)
	DIRS += libintl
endif

ifeq ($(HAVE_DOT_CONFIG),y)

all: finished

# In this section, we need .config
-include .config.cmd

finished: subdirs
	$(SECHO)
	$(SECHO) Finally finished compiling ...
	$(SECHO)

include/bits/uClibc_config.h: .config
	@if [ ! -x ./extra/config/conf ] ; then \
	    $(MAKE) -C extra/config conf; \
	fi
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -o extra/Configs/Config.in

# For the moment, we have to keep re-running this target 
# because the fix includes scripts rely on pre-processers 
# in order to generate the headers correctly :(.  That 
# means we can't use the $(HOSTCC) in order to get the 
# correct output.
ifeq ($(ARCH_HAS_MMU),y)
export header_extra_args = 
else
export header_extra_args = -n
endif
headers: include/bits/uClibc_config.h
	@$(SHELL_SET_X); \
	./extra/scripts/fix_includes.sh \
		-k $(KERNEL_SOURCE) -t $(TARGET_ARCH) \
		$(header_extra_args)
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
	set -e; \
	$(SHELL_SET_X); \
	TOPDIR=. CC="$(CC)" /bin/sh extra/scripts/gen_bits_syscall_h.sh > include/bits/sysnum.h.new; \
	if cmp include/bits/sysnum.h include/bits/sysnum.h.new >/dev/null 2>&1; then \
		$(RM) include/bits/sysnum.h.new; \
	else \
		mv -f include/bits/sysnum.h.new include/bits/sysnum.h; \
	fi
ifeq ($(UCLIBC_HAS_THREADS),y)
	$(MAKE) -C libpthread headers
endif
	$(MAKE) -C libc/sysdeps/linux/common headers
	$(MAKE) -C libc/sysdeps/linux/$(TARGET_ARCH) headers

# Command used to download source code
WGET:=wget --passive-ftp

LOCALE_DATA_FILENAME:=uClibc-locale-030818.tgz

pregen: headers
ifeq ($(UCLIBC_DOWNLOAD_PREGENERATED_LOCALE_DATA),y)
	(cd extra/locale; \
	if [ ! -f $(LOCALE_DATA_FILENAME) ] ; then \
	$(WGET) http://www.uclibc.org/downloads/$(LOCALE_DATA_FILENAME) ; \
	fi )
endif
ifeq ($(UCLIBC_PREGENERATED_LOCALE_DATA),y)
	(cd extra/locale; zcat $(LOCALE_DATA_FILENAME) | tar -xvf -)
	$(MAKE) -C extra/locale pregen
endif

pre_subdirs: $(patsubst %, _pre_dir_%, $(PRE_DIRS))
$(patsubst %, _pre_dir_%, $(PRE_DIRS)): pregen
	$(MAKE) -C $(patsubst _pre_dir_%, %, $@)

subdirs: $(patsubst %, _dir_%, $(DIRS))
$(patsubst %, _dir_%, $(DIRS)): pre_subdirs
	$(MAKE) -C $(patsubst _dir_%, %, $@)

tags:
	ctags -R

install: install_runtime install_dev finished2


RUNTIME_PREFIX_LIB_FROM_DEVEL_PREFIX_LIB=$(shell extra/scripts/relative_path.sh $(DEVEL_PREFIX)lib $(RUNTIME_PREFIX)lib)

# Installs header files.
install_headers:
	$(INSTALL) -d $(PREFIX)$(DEVEL_PREFIX)include
	if [ "$(KERNEL_SOURCE)" == "$(DEVEL_PREFIX)" ] ; then \
		extra_exclude="--exclude include/linux --exclude include/asm'*'" ; \
	else \
		extra_exclude="" ; \
	fi ; \
	tar -chf - include --exclude .svn --exclude CVS $$extra_exclude \
		| tar -xf - -C $(PREFIX)$(DEVEL_PREFIX)
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/ssp-internal.h
ifneq ($(UCLIBC_HAS_FLOATS),y)
	# Remove floating point related headers since float support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/complex.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/fpu_control.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/ieee754.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/math.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/tgmath.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/bits/uClibc_fpmax.h
endif
ifneq ($(UCLIBC_HAS_WCHAR),y)
	# Remove wide char headers since wide char support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/wctype.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/wchar.h
endif
ifneq ($(UCLIBC_HAS_LOCALE),y)
	# Remove iconv header since locale support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/iconv.h
endif
ifneq ($(UCLIBC_HAS_GLIBC_CUSTOM_PRINTF),y)
	# Remove printf header since custom print specifier support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/printf.h
endif
ifneq ($(UCLIBC_HAS_XLOCALE),y)
	# Remove xlocale header since extended locale support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/xlocale.h
endif
ifneq ($(UCLIBC_HAS_GETTEXT_AWARENESS),y)
	# Remove libintl header since gettext support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/libintl.h
endif
ifneq ($(UCLIBC_HAS_REGEX),y)
	# Remove regex headers since regex support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/regex.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/regexp.h
endif
ifneq ($(UCLIBC_HAS_WORDEXP),y)
	# Remove wordexp header since wordexp support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/wordexp.h
endif
ifneq ($(UCLIBC_HAS_FTW),y)
	# Remove ftw header since ftw support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/ftw.h
endif
ifneq ($(UCLIBC_HAS_GLOB),y)
	# Remove glob header since glob support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/glob.h
endif
ifneq ($(UCLIBC_HAS_GNU_GETOPT),y)
	# Remove getopt header since gnu getopt support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/getopt.h
endif
ifneq ($(HAS_SHADOW),y)
	# Remove shadow header since shadow password support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/shadow.h
endif
ifneq ($(PTHREADS_DEBUG_SUPPORT),y)
	# Remove thread_db header since thread debug support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/thread_db.h
endif
ifneq ($(UCLIBC_HAS_THREADS),y)
	# Remove pthread headers since thread support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/*thread*.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/semaphore.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/bits/*thread*.h
endif
	-@for i in `find  $(PREFIX)$(DEVEL_PREFIX) -type d` ; do \
	    chmod 755 $$i; chmod 644 $$i/*.h > /dev/null 2>&1; \
	done
	-find $(PREFIX)$(DEVEL_PREFIX) -name .svn | xargs $(RM) -r
	-chown -R `id | sed 's/^uid=\([0-9]*\).*gid=\([0-9]*\).*$$/\1:\2/'` $(PREFIX)$(DEVEL_PREFIX)

# Installs development library links.
install_dev: install_headers
	$(INSTALL) -d $(PREFIX)$(DEVEL_PREFIX)lib
	-$(INSTALL) -m 644 lib/*.[ao] $(PREFIX)$(DEVEL_PREFIX)lib/
ifeq ($(HAVE_SHARED),y)
	for i in `find lib/ -type l -name 'lib[a-zA-Z]*.so' | \
	sed -e 's/lib\///'` ; do \
		$(LN) -sf $(RUNTIME_PREFIX_LIB_FROM_DEVEL_PREFIX_LIB)$$i.$(MAJOR_VERSION) \
		$(PREFIX)$(DEVEL_PREFIX)lib/$$i; \
	done
	if [ -f $(TOPDIR)lib/libc.so -a -f $(PREFIX)$(RUNTIME_PREFIX)lib/$(SHARED_MAJORNAME) ] ; then \
		$(RM) $(PREFIX)$(DEVEL_PREFIX)lib/libc.so; \
		sed -e '/^GROUP/d' $(TOPDIR)lib/libc.so > $(PREFIX)$(DEVEL_PREFIX)lib/libc.so; \
	fi
ifeq ($(COMPAT_ATEXIT),y)
	if [ -f $(TOPDIR)lib/libc.so -a -f $(PREFIX)$(RUNTIME_PREFIX)lib/$(SHARED_MAJORNAME) ] ; then \
		echo "GROUP ( $(DEVEL_PREFIX)lib/$(NONSHARED_LIBNAME) $(RUNTIME_PREFIX)lib/$(SHARED_MAJORNAME) )" \
			>> $(PREFIX)$(DEVEL_PREFIX)lib/libc.so; \
	fi
else
	if [ -f $(TOPDIR)lib/libc.so -a -f $(PREFIX)$(RUNTIME_PREFIX)lib/$(SHARED_MAJORNAME) ] ; then \
		echo "GROUP ( $(RUNTIME_PREFIX)lib/$(SHARED_MAJORNAME) $(DEVEL_PREFIX)lib/$(NONSHARED_LIBNAME) )" \
			>> $(PREFIX)$(DEVEL_PREFIX)lib/libc.so; \
	fi
endif
ifeq ($(PTHREADS_DEBUG_SUPPORT),y)
	$(LN) -sf $(RUNTIME_PREFIX_LIB_FROM_DEVEL_PREFIX_LIB)libthread_db.so.1 \
		$(PREFIX)$(DEVEL_PREFIX)lib/libthread_db.so
endif
#	# If we build shared libraries then the static libs are PIC...
#	# Make _pic.a symlinks to make mklibs.py and similar tools happy.
	if [ -d lib ] ; then \
		for i in `find lib/  -type f -name 'lib*.a' | sed -e 's/lib\///'` ; do \
			$(LN) -sf $$i $(PREFIX)$(DEVEL_PREFIX)lib/`echo $$i \
				| sed -e 's/\.a$$/_pic.a/'`; \
		done ; \
	fi
endif

# Installs run-time libraries
install_runtime:
ifeq ($(HAVE_SHARED),y)
	$(INSTALL) -d $(PREFIX)$(RUNTIME_PREFIX)lib
	$(INSTALL) -m 644 lib/lib*-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
		$(PREFIX)$(RUNTIME_PREFIX)lib
	cp -dRf lib/*.so.* $(PREFIX)$(RUNTIME_PREFIX)lib
	@if [ -x lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so ] ; then \
	    set -e; \
		$(SHELL_SET_X); \
	    $(INSTALL) -m 755 lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
	    		$(PREFIX)$(RUNTIME_PREFIX)lib; \
	fi
endif

utils:
	$(MAKE) CROSS="$(CROSS)" CC="$(CC)" -C utils

# Installs helper applications, such as 'ldd' and 'ldconfig'
install_utils: utils
	$(MAKE) CROSS="$(CROSS)" CC="$(CC)" -C utils install

finished2:
	$(SECHO)
	$(SECHO) Finished installing ...
	$(SECHO)

else # ifeq ($(HAVE_DOT_CONFIG),y)

all: menuconfig

# configuration
# ---------------------------------------------------------------------------
extra/config/conf:
	$(MAKE) -C extra/config conf

extra/config/mconf:
	$(MAKE) -C extra/config ncurses mconf

menuconfig: extra/config/mconf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/mconf extra/Configs/Config.in
	$(MAKE) headers

config: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf extra/Configs/Config.in

oldconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -o extra/Configs/Config.in

randconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -r extra/Configs/Config.in

allyesconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -y extra/Configs/Config.in
	sed -i -e "s/^DODEBUG=.*/# DODEBUG is not set/" .config
	sed -i -e "s/^DOASSERTS=.*/# DOASSERTS is not set/" .config
	sed -i -e "s/^SUPPORT_LD_DEBUG_EARLY=.*/# SUPPORT_LD_DEBUG_EARLY is not set/" .config
	sed -i -e "s/^SUPPORT_LD_DEBUG=.*/# SUPPORT_LD_DEBUG is not set/" .config
	sed -i -e "s/^UCLIBC_MJN3_ONLY=.*/# UCLIBC_MJN3_ONLY is not set/" .config
	@./extra/config/conf -o extra/Configs/Config.in

allnoconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -n extra/Configs/Config.in

defconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -d extra/Configs/Config.in

clean:
	@$(RM) -r lib include/bits
	$(RM) libc/*.a libc/obj.* libc/nonshared_obj.*
	$(RM) libc/misc/locale/locale_data.c
	$(RM) libc/misc/internals/interp.c
	$(RM) ldso/libdl/*.a
	$(RM) include/fpu_control.h
	$(MAKE) -C extra/locale clean
	$(MAKE) -C ldso headers_clean
	$(MAKE) -C libpthread headers_clean
	$(MAKE) -C test clean
	$(MAKE) -C utils clean
	@set -e; \
	for i in `(cd $(TOPDIR)/libc/sysdeps/linux/common/sys; ls *.h)` ; do \
		$(RM) include/sys/$$i; \
	done; \
	if [ -d libc/sysdeps/linux/$(TARGET_ARCH)/sys ] ; then \
		for i in `(cd libc/sysdeps/linux/$(TARGET_ARCH)/sys; ls *.h)` ; do \
			$(RM) include/sys/$$i; \
		done; \
	fi
	@$(RM) include/linux include/asm*
	@if [ -d libc/sysdeps/linux/$(TARGET_ARCH) ]; then		\
	    $(MAKE) -C libc/sysdeps/linux/$(TARGET_ARCH) clean;		\
	fi
	-find . \( -name \*.o -o -name \*.os \) -exec $(RM) {} \;

distclean: clean
	-find . \( -name core -o -name \*.orig -o -name \*~ -o -name .\#\* \) -exec $(RM) {} \;
	$(RM) .config .config.old .config.cmd
	$(RM) extra/locale/*.txt
	$(MAKE) -C extra clean

release: distclean
	cd ..;					\
	$(RM) -r uClibc-$(VERSION);		\
	cp -dRf uClibc uClibc-$(VERSION);	\
	find uClibc-$(VERSION)/ -type f		\
	    -name .\#* -exec $(RM) -r {} \; ;	\
	find uClibc-$(VERSION)/ -type d		\
	    -name .svn -exec $(RM) -r {} \; ;	\
						\
	tar -cvzf uClibc-$(VERSION).tar.gz uClibc-$(VERSION)/

endif # ifeq ($(HAVE_DOT_CONFIG),y)

check:
	$(MAKE) -C test

.PHONY: dummy subdirs release distclean clean config oldconfig menuconfig utils
