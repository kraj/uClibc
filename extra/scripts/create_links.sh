#! /bin/sh
#
# Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
#
# Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
#

# Creates the necessary links to build w/ the Makefile.in files

DIRS="ldso libc libcrypt libintl libm libnsl libpthread libresolv librt libutil"
DIRS2="ldso/ldso ldso/libdl libpthread/linuxthreads libpthread/linuxthreads_db"

for x in ${DIRS} ; do
	find ${x} -type f -name Makefile -exec rm -f {} \;
	ln -sf ../extra/scripts/Makefile.libs.lvl1 ${x}/Makefile
done

for x in ${DIRS2} ; do
	ln -sf ../../extra/scripts/Makefile.libs.lvl2 ${x}/Makefile
done

ln -sf ../../../extra/scripts/Makefile.objs.lvl3 libc/sysdeps/linux/Makefile
ln -sf ../../extra/scripts/Makefile.objs.lvl2 libc/string/Makefile

rm -f Makefile
ln -sf Makefile.in Makefile

exit 0
