# uClibc time tests
# Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.

TESTS_DISABLED := bug-asctime bug-asctime_r time tst-mktime2 tst-posixtz \
	tst-strftime tst-strptime tst-timezone

ifneq ($(UCLIBC_HAS_XLOCALE),y)
TESTS_DISABLED += tst-ftime_l
endif

include ../Test.mak

CFLAGS_tst-strptime2 := -std=c99
