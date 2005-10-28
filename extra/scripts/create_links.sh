#! /bin/sh
#
# Creates the minimally necessary links to build w/ the Makefile.in files

DIRS="ldso libc libcrypt libintl libm libnsl libpthread libresolv librt libutil"
DIRS2="ldso/ldso ldso/libdl libpthread/linuxthreads libpthread/linuxthreads_db"
OBJ4="common arm i386 mips powerpc x86_64"

for x in ${DIRS} ; do
	find ${x} -type f -name Makefile -exec rm -f {} \;
	ln -sf ../extra/scripts/Makefile.libs.lvl1 ${x}/Makefile
done

for x in ${DIRS2} ; do
	ln -sf ../../extra/scripts/Makefile.libs.lvl2 ${x}/Makefile
done

for x in ${OBJ4} ; do
	ln -sf ../../../../extra/scripts/Makefile.objs.lvl4 libc/sysdeps/linux/${x}/Makefile
done

rm -f Makefile
ln -sf Makefile.in Makefile

exit 0
