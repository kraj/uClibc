#!/bin/sh
#
# Copyright (C) 2001 Manuel Novoa III <mjn3@uclibc.org>
# Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
#
# Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
#

# June 27, 2001         Manuel Novoa III
#
# This script expects top_builddir and CC (as used in the Makefiles) to be set in
# the environment, and outputs the appropriate $top_builddir/include/bits/sysnum.h
# corresponding to $top_builddir/include/asm/unistd.h to stdout.
#
# Warning!!! This does _no_ error checking!!!

UNISTD_H_PATH=$top_builddir/include/asm/unistd.h
INCLUDE_OPTS="-I$top_builddir/include"

case $CC in
*icc*) CC_SYSNUM_ARGS="-dM" ;;
*)     CC_SYSNUM_ARGS="-dN" ;;
esac

( echo "#include \"$UNISTD_H_PATH\"" ;
  $CC -E $CC_SYSNUM_ARGS $INCLUDE_OPTS $UNISTD_H_PATH | # needed to strip out any kernel-internal defines
  sed -ne 's/^[ ]*#define[ ]*__NR_\([A-Za-z0-9_]*\).*/UCLIBC_\1 __NR_\1/gp'
) |
$CC -E $INCLUDE_OPTS - |
( echo "/* WARNING!!! AUTO-GENERATED FILE!!! DO NOT EDIT!!! */" ; echo ;
  echo "#ifndef _BITS_SYSNUM_H" ;
  echo "#define _BITS_SYSNUM_H" ;
  echo ;
  echo "#ifndef _SYSCALL_H" ;
  echo "# error \"Never use <bits/sysnum.h> directly; include <sys/syscall.h> instead.\"" ;
  echo "#endif" ; echo ;
  sed -ne 's/^UCLIBC_\([A-Za-z0-9_]*\) *\(.*\)/#undef __NR_\1\
#define __NR_\1 \2\
#define SYS_\1 __NR_\1/gp'
  echo ;
  echo "#endif" ;
)
