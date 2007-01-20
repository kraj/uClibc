# Common makefile rules for tests
#
# Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
#
# Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.

ifeq ($(TESTS),)
TESTS := $(patsubst %.c,%,$(wildcard *.c))
endif
ifneq ($(TESTS_DISABLED),)
TESTS := $(filter-out $(TESTS_DISABLED),$(TESTS))
endif
ifeq ($(SHELL_TESTS),)
SHELL_TESTS := $(patsubst %.sh,shell_%,$(wildcard *.sh))
endif

ifneq ($(filter-out test,$(TESTS)),$(TESTS))
$(error Sanity check: cannot have a test named "test.c")
endif

top_builddir = ../../
include ../Rules.mak

U_TARGETS := $(TESTS)
G_TARGETS := $(patsubst %,%_glibc,$(U_TARGETS))
U_TARGETS += $(U_TESTS)
G_TARGETS += $(G_TESTS)

TARGETS   := $(SHELL_TESTS)
ifeq ($(GLIBC_ONLY),)
TARGETS   += $(U_TARGETS)
endif
ifeq ($(UCLIBC_ONLY),)
TARGETS   += $(G_TARGETS)
endif
CLEAN_TARGETS := $(U_TARGETS) $(G_TARGETS)

test check all: $(TARGETS)

$(TARGETS): Makefile $(TESTDIR)Makefile $(TESTDIR)Rules.mak $(TESTDIR)Test.mak
$(U_TARGETS): $(patsubst %,%.c,$(U_TARGETS))
$(G_TARGETS): $(patsubst %_glibc,%.c,$(G_TARGETS))

define diff_test
	$(Q)\
	for x in "$@.out" "$(patsubst %_glibc,%,$@).out" ; do \
		test -e "$$x.good" && $(do_showdiff) "$@.out" "$$x.good" && exec diff -u "$@.out" "$$x.good" ; \
	done ; \
	true
endef
define uclibc_glibc_diff_test
	$(Q)\
	test -z "$(DODIFF_$(patsubst %_glibc,%,$@))" && exec true ; \
	uclibc_out="$@.out" ; \
	glibc_out="$(patsubst %_glibc,%,$@).out" ; \
	$(do_showdiff) $$uclibc_out $$glibc_out ; \
	exec diff -u "$$uclibc_out" "$$glibc_out"
endef
define exec_test
	$(showtest)
	$(Q)\
	$(WRAPPER) $(WRAPPER_$(patsubst %_glibc,%,$@)) \
	./$@ $(OPTS) $(OPTS_$(patsubst %_glibc,%,$@)) &> "$@.out" ; \
		ret=$$? ; \
		expected_ret="$(RET_$(patsubst %_glibc,%,$@))" ; \
		test -z "$$expected_ret" && export expected_ret=0 ; \
	if ! test $$ret -eq $$expected_ret ; then \
		$(RM) $@ ; \
		cat "$@.out" ; \
		exec false ; \
	fi
	$(SCAT) "$@.out"
endef

$(U_TARGETS):
ifeq ($(CHECK_ONLY),)
	$(showlink)
	$(Q)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -c $@.c -o $@.o
	$(Q)$(CC) $(LDFLAGS) $@.o -o $@ $(EXTRA_LDFLAGS) $(LDFLAGS_$@)
endif
ifeq ($(COMPILE_ONLY),)
	$(exec_test)
	$(diff_test)
endif

$(G_TARGETS):
	$(showlink)
	$(Q)$(HOSTCC) $(HOST_CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$(patsubst %_glibc,%,$@)) -c $(patsubst %_glibc,%,$@).c -o $@.o
	$(Q)$(HOSTCC) $(HOST_LDFLAGS) $@.o -o $@ $(EXTRA_LDFLAGS) $(LDFLAGS_$(patsubst %_glibc,%,$@))
ifeq ($(COMPILE_ONLY),)
	$(exec_test)
	$(diff_test)
	$(uclibc_glibc_diff_test)
endif

shell_%:
ifeq ($(COMPILE_ONLY),)
	$(showtest)
	$(Q)$(SHELL) $(patsubst shell_%,%.sh,$@)
endif

%.so: %.c
	$(showlink)
	$(Q)$(CC) \
		$(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$(patsubst %_glibc,%,$@)) \
		-fPIC -shared $< -o $@ -Wl,-soname,$@ \
		$(LDFLAGS) $(EXTRA_LIBS) $(LDFLAGS_$(patsubst %_glibc,%,$@))

clean:
	$(showclean)
	$(Q)$(RM) *.a *.o *.so *~ core *.out *.gdb $(CLEAN_TARGETS) $(EXTRA_CLEAN)

.PHONY: all check clean test
