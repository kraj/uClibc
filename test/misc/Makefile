# uClibc misc tests
# Licensed under the GNU Library General Public License, see COPYING.LIB

TESTS := dirent dirent64 fdopen sem tst-fnmatch #outb

include ../Test.mak

CFLAGS_dirent64 := -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

DODIFF_dirent    := 1
DODIFF_dirent64  := 1

OPTS_tst-fnmatch := < tst-fnmatch.input
