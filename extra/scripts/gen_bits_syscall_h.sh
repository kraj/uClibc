#!/bin/sh
#
# June 27, 2001         Manuel Novoa III
#
# This script expects TOPDIR and CC (as used in the Makefiles) to be set in
# the environment, and outputs the appropriate $TOPDIR/include/bits/syscall.h
# corresponding to $TOPDIR/include/asm/unistd.h to stdout.
#
# Warning!!! This does _no_ error checking!!!

UNISTD_H_PATH=$TOPDIR/include/asm/unistd.h

( echo "#include \"$UNISTD_H_PATH\"" ;
  $CC -E -dN $UNISTD_H_PATH | # needed to strip out any kernel-internal defines
  sed -ne 's/^[ ]*#define[ ]*__NR_\([A-Za-z0-9_]*\).*/UCLIBC_\1 __NR_\1/gp'
) |
$CC -E - |
( echo "/* WARNING!!! AUTO-GENERATED FILE!!! DO NOT EDIT!!! */" ; echo ;
  echo "#ifndef _BITS_SYSCALL_H" ;
  echo "#define _BITS_SYSCALL_H" ;
  echo "#ifndef _SYSCALL_H" ;
  echo "# error \"Never use <bits/syscall.h> directly; include <sys/syscall.h> instead.\"" ;
  echo "#endif" ; echo ;
  sed -ne 's/^UCLIBC_\([A-Za-z0-9_]*\) *\(.*\)/#define __NR_\1 \2\
#define SYS_\1 __NR_\1/gp'
  echo "#endif" ; echo ;
)
