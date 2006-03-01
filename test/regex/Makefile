# uClibc regex tests
# Licensed under the GNU Library General Public License, see COPYING.LIB

TESTS_DISABLED := testregexn

include ../Test.mak

CFLAGS_tst-regex2 := -std=c99

CFLAGS-OMIT_testregex.c := -D_GNU_SOURCE
CFLAGS-OMIT_testregexc.c := -D_GNU_SOURCE
CFLAGS-OMIT_testregexf.c := -D_GNU_SOURCE
CFLAGS-OMIT_testregexl.c := -D_GNU_SOURCE
CFLAGS-OMIT_testregexn.c := -D_GNU_SOURCE
CFLAGS-OMIT_testregexp.c := -D_GNU_SOURCE
CFLAGS-OMIT_testregexr.c := -D_GNU_SOURCE

OPTS_testregex := < basic.dat
OPTS_testregexc := < categorize.dat
OPTS_testregexf := < forcedassoc.dat
OPTS_testregexl := < leftassoc.dat
OPTS_testregexn := < nullsubexpr.dat
OPTS_testregexp := < repetition.dat
OPTS_testregexr := < rightassoc.dat
