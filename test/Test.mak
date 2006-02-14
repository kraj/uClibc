# Common makefile rules for tests
#
# Copyright (C) 2000,2001 Erik Andersen <andersen@uclibc.org>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Library General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for more
# details.
#
# You should have received a copy of the GNU Library General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

include ../Rules.mak

U_TARGETS := $(TESTS)
G_TARGETS := $(patsubst %,%_glibc,$(U_TARGETS))
U_TARGETS += $(U_TESTS)
G_TARGETS += $(G_TESTS)

TARGETS    = 
ifeq ($(GLIBC_ONLY),)
TARGETS   += $(U_TARGETS)
endif
ifeq ($(UCLIBC_ONLY),)
TARGETS   += $(G_TARGETS)
endif

test check all: $(TARGETS)
# dummy rule to prevent the "Nothing to be done for `all'." message
ifeq ($(Q),@)
	@true
endif

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
	$(showtest)
	$(Q)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -c $@.c -o $@.o
	$(Q)$(CC) $(LDFLAGS) $@.o -o $@ $(EXTRA_LDFLAGS) $(LDFLAGS_$@)
ifeq ($(COMPILE_ONLY),)
	$(exec_test)
	$(diff_test)
endif

$(G_TARGETS):
	$(showtest)
	$(Q)$(HOSTCC) $(HOST_CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$(patsubst %_glibc,%,$@)) -c $(patsubst %_glibc,%,$@).c -o $@.o
	$(Q)$(HOSTCC) $(HOST_LDFLAGS) $@.o -o $@ $(EXTRA_LDFLAGS) $(LDFLAGS_$(patsubst %_glibc,%,$@))
ifeq ($(COMPILE_ONLY),)
	$(exec_test)
	$(diff_test)
	$(uclibc_glibc_diff_test)
endif

%.so: %.c
	$(showlink)
	$(Q)$(CC) \
		$(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$(patsubst %_glibc,%,$@)) \
		-fPIC -shared $< -o $@ -Wl,-soname,$@ \
		$(LDFLAGS) $(EXTRA_LIBS) $(LDFLAGS_$(patsubst %_glibc,%,$@))

clean:
	$(showclean)
	$(Q)$(RM) *.a *.o *.so *~ core *.out $(TARGETS)
