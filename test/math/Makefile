# uClibc math tests
# Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.

include ../../.config

TESTS := basic-test rint tst-definitions test-fpucw test-float test-ifloat test-double test-idouble
ifeq ($(UCLIBC_HAS_LONG_DOUBLE_MATH),y)
TESTS += test-ldouble test-ildoubl compile_test
else
CFLAGS_basic-test := -DNO_LONG_DOUBLE
endif

include ../Test.mak

DODIFF_rint     := 1

# NOTE: For basic-test we must disable the floating point optimization.
#       Only for sh architecture because in the other architecture are disabled.
ifeq ($(TARGET_ARCH),sh)
CFLAGS_basic-test += -mieee
endif
EXTRA_CFLAGS    := -fno-builtin
EXTRA_LDFLAGS   := -lm

PERL := /usr/bin/perl
ulps-file := $(firstword $(wildcard $(config-sysdirs:%=$(..)%/libm-test-ulps)))
libm-test.c: $(ulps-file) libm-test.inc gen-libm-test.pl
	$(Q)$(PERL) ./gen-libm-test.pl -u $< ./libm-test.inc -o "." 2>&1 > /dev/null
EXTRA_CLEAN := libm-test.c libm-test-ulps.h
$(TARGETS): libm-test.c
