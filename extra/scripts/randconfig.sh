#!/bin/sh

test "x$ARCH" = "x" && ARCH=`uname -m`
KCONFIG_ALLCONFIG=.config.allconfig
(echo TARGET_$ARCH=y
 echo '# UCLIBC_PREGENERATED_LOCALE_DATA is not set'
 echo '# UCLIBC_DOWNLOAD_PREGENERATED_LOCALE_DATA is not set'
) > $KCONFIG_ALLCONFIG
export KCONFIG_ALLCONFIG

i=0
while test ! -f STOP
do
  make $* randconfig > /dev/null
  make $* silentoldconfig > /dev/null
  if (make $*) 2>&1 >& mk.log
  then
    :
  else
    i=`expr $i + 1`
    num=`printf "%.5d" $i`
    mv .config FAILED.$num.config
    mv mk.log FAILED.$num.log
  fi
  make distclean > /dev/null || true
done
