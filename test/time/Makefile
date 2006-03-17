# uClibc time tests
# Licensed under the GNU Library General Public License, see COPYING.LIB

TESTS_DISABLED := bug-asctime bug-asctime_r time tst-mktime2 tst-posixtz \
	tst-strftime tst-strptime tst-timezone

ifneq ($(UCLIBC_HAS_LOCALE),y)
TESTS_DISABLED += tst-ftime_l
endif

include ../Test.mak

CFLAGS_tst-strptime2 := -std=c99
