# uClibc stat tests
# Licensed under the GNU Library General Public License, see COPYING.LIB

include ../Test.mak

CFLAGS_stat64 := -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

DODIFF_stat   := 1
DODIFF_stat64 := 1

OPTS_stat   := stat.c
OPTS_stat64 := stat64.c

# make sure the change in atime doesn't screw us up
WRAPPER_stat   := touch -r . stat.c ;
WRAPPER_stat64 := touch -r . stat64.c ;
