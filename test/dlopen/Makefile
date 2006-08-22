# uClibc dlopen tests
# Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.

# rules need a little love to work with glibc ...
export UCLIBC_ONLY := 1

TESTS := dltest dltest2 test1 test2 test3

include ../Test.mak

CFLAGS_dltest   := -DLIBNAME="\"./libtest.so\""
CFLAGS_dltest2  := -DLIBNAME="\"./libtest3.so\""

LDFLAGS_dltest  := -ldl -lpthread
LDFLAGS_dltest2 := -ldl -lpthread
LDFLAGS_test1   := -ldl
LDFLAGS_test2   := -ldl
LDFLAGS_test3   := -ldl ./libtest1.so ./libtest2.so -Wl,-rpath,.

DEBUG_LIBS := X
WRAPPER := env $(DEBUG_LIBS)=all LD_LIBRARY_PATH="$$PWD:.:$(LD_LIBRARY_PATH)"

dltest: libtest.so
dltest2: libtest3.so
test1: libtest1.so
test2: libtest1.so libtest2.so
test3: libtest1.so libtest2.so
libtest1.so: libtest2.so
LDFLAGS_libtest1.so := ./libtest2.so -Wl,-rpath,.
LDFLAGS_libtest2.so := -Wl,-rpath,.
LDFLAGS_libtest3.so := -lpthread -Wl,-rpath,.
