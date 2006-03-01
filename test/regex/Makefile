# uClibc regex tests
# Licensed under the GNU Library General Public License, see COPYING.LIB

TESTS_DISABLED := testregexn

include ../Test.mak

CFLAGS_tst-regex2 := -std=c99

CFLAGS-OMIT_testregex := -D_GNU_SOURCE
CFLAGS-OMIT_testregexc := -D_GNU_SOURCE
CFLAGS-OMIT_testregexf := -D_GNU_SOURCE
CFLAGS-OMIT_testregexl := -D_GNU_SOURCE
CFLAGS-OMIT_testregexn := -D_GNU_SOURCE
CFLAGS-OMIT_testregexp := -D_GNU_SOURCE
CFLAGS-OMIT_testregexr := -D_GNU_SOURCE

OPTS_testregex := < basic.dat
OPTS_testregexc := < categorize.dat
OPTS_testregexf := < forcedassoc.dat
OPTS_testregexl := < leftassoc.dat
OPTS_testregexn := < nullsubexpr.dat
OPTS_testregexp := < repetition.dat
OPTS_testregexr := < rightassoc.dat
