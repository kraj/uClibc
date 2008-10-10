obj := .
src := .
top_srcdir=../../
top_builddir=../../
srctree := .
include $(top_builddir)Rules.mak

include Makefile.kconfig

ifeq ($(findstring mconf,$(MAKECMDGOALS)),mconf)
hostprogs-y += mconf
endif

-include .depend
.depend: $(wildcard *.h *.c)
	$(Q)$(HOSTCC) $(HOST_EXTRACFLAGS) $(HOSTCFLAGS) -MM *.c > .depend 2>/dev/null || :

__hostprogs := $(sort $(hostprogs-y) $(hostprogs-m))
host-csingle    := $(foreach m,$(__hostprogs),$(if $($(m)-objs),,$(m)))
host-cmulti := $(foreach m,$(__hostprogs),\
           $(if $($(m)-cxxobjs),,$(if $($(m)-objs),$(m))))
host-cobjs  := $(sort $(foreach m,$(__hostprogs),$($(m)-objs)))

$(host-csingle): %: %.c
	$(HOSTCC) $(HOST_EXTRACFLAGS) $(HOSTCFLAGS) $(HOSTCFLAGS_$@) $< $(HOST_LOADLIBES) -o $@

$(host-cmulti): %: $(host-cobjs) $(host-cshlib)
	$(HOSTCC) $(HOST_EXTRACFLAGS) $(HOSTCFLAGS) $(HOSTCFLAGS_$@) $($@-objs) $(HOST_LOADLIBES) -o $@

$(host-cobjs): %.o: %.c
	$(HOSTCC) $(HOST_EXTRACFLAGS) $(HOSTCFLAGS) $(HOSTCFLAGS_$@) -c $< -o $@

$(obj)/%:: $(src)/%_shipped
	$(Q)cat $< > $@

clean:
	$(Q)rm -f $(clean-files) conf
distclean: clean
	$(Q)rm -f $(lxdialog) $(conf-objs) $(mconf-objs) $(kxgettext-objs) \
		$(hostprogs-y) $(qconf-cxxobjs) $(qconf-objs) $(gconf-objs) \
		.depend
	$(Q)rm -r -f $(top_builddir)include/config

FORCE:
.PHONY: FORCE clean distclean
