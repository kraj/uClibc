# uClibc misc tests
# Licensed under the GNU Library General Public License, see COPYING.LIB

TESTS_DISABLED := outb tst-fnmatch bug-glob1

include ../Test.mak

CFLAGS_dirent64 := -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

DODIFF_dirent    := 1
DODIFF_dirent64  := 1

OPTS_bug-glob1   := $(PWD)
OPTS_tst-fnmatch := < tst-fnmatch.input
