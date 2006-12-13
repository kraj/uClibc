# uClibc TLS tests
# Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.

TESTS := tst-tls1 tst-tls2 tst-tls3 tst-tls4 tst-tls5 tst-tls6 tst-tls7	\
	tst-tls8 tst-tls9 tst-tls10 tst-tls11 tst-tls12 tst-tls13	\
	tst-tls14 tst-tls15 tst-tls1-static tst-tls2-static		\
	tst-tls9-static
TESTS_DISABLED := tst-tls1-static tst-tls2-static tst-tls9-static

include ../Test.mak

TARGET_ARCH := $(strip $(subst ",, $(strip $(TARGET_ARCH))))
PTDIR := $(top_builddir)libpthread/nptl

EXTRA_CFLAGS := -DNOT_IN_libc=1 -DIS_IN_libpthread=1 -D_LIBC	\
	-std=gnu99 -I. -I$(PTDIR)				\
	-I$(PTDIR)/sysdeps/unix/sysv/linux/$(TARGET_ARCH)	\
	-I$(PTDIR)/sysdeps/$(TARGET_ARCH)			\
	-I$(PTDIR)/sysdeps/unix/sysv/linux			\
	-I$(PTDIR)/sysdeps/pthread				\
	-I$(PTDIR)/sysdeps/pthread/bits				\
	-I$(PTDIR)/sysdeps/generic				\
	-I$(top_builddir)ldso/include				\
	-I$(top_builddir)ldso/ldso/$(TARGET_ARCH)		\
	-include $(top_builddir)include/libc-symbols.h

CFLAGS_tst-tlsmod1.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod2.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod3.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod4.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod5.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod6.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod7.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod8.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod9.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod10.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod11.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod12.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod13.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod13a.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod14a.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod14b.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod15a.so := -fPIC -DSHARED -shared
CFLAGS_tst-tlsmod15b.so := -fPIC -DSHARED -shared

LDFLAGS_tst-tlsmod1.so := -shared -static-libgcc -L$(top_builddir)lib
LDFLAGS_tst-tlsmod2.so := -shared -static-libgcc -L$(top_builddir)lib
LDFLAGS_tst-tlsmod3.so := -shared -static-libgcc -L$(top_builddir)lib	\
	tst-tlsmod2.so
LDFLAGS_tst-tlsmod4.so := -shared -static-libgcc -L$(top_builddir)lib
LDFLAGS_tst-tlsmod5.so := -shared -static-libgcc -L$(top_builddir)lib
LDFLAGS_tst-tlsmod6.so := -shared -static-libgcc -L$(top_builddir)lib
LDFLAGS_tst-tlsmod7.so := -shared -static-libgcc -L$(top_builddir)lib
LDFLAGS_tst-tlsmod8.so := -shared -static-libgcc -L$(top_builddir)lib	\
	tst-tlsmod7.so
LDFLAGS_tst-tlsmod9.so := -shared -static-libgcc -L$(top_builddir)lib
LDFLAGS_tst-tlsmod10.so := -shared -static-libgcc -L$(top_builddir)lib	\
	tst-tlsmod9.so
LDFLAGS_tst-tlsmod11.so := -shared -static-libgcc -L$(top_builddir)lib
LDFLAGS_tst-tlsmod12.so := -shared -static-libgcc -L$(top_builddir)lib	\
	tst-tlsmod11.so
LDFLAGS_tst-tlsmod13.so := -shared -static-libgcc -L$(top_builddir)lib
LDFLAGS_tst-tlsmod13a.so := -shared -static-libgcc -L$(top_builddir)lib	\
	tst-tlsmod13.so
LDFLAGS_tst-tlsmod14a.so := -shared -static-libgcc -L$(top_builddir)lib
LDFLAGS_tst-tlsmod14b.so := -shared -static-libgcc -L$(top_builddir)lib
LDFLAGS_tst-tlsmod15a.so := -shared -static-libgcc -L$(top_builddir)lib
LDFLAGS_tst-tlsmod15b.so := -shared -static-libgcc -L$(top_builddir)lib

LDFLAGS_tst-tls3 := tst-tlsmod1.so
LDFLAGS_tst-tls4 := -ldl
LDFLAGS_tst-tls5 := -ldl
LDFLAGS_tst-tls6 := -ldl
LDFLAGS_tst-tls7 := -ldl
LDFLAGS_tst-tls8 := -ldl
LDFLAGS_tst-tls9 := -ldl
LDFLAGS_tst-tls10 := -Wl,-rpath-link=. tst-tlsmod8.so
LDFLAGS_tst-tls11 := -Wl,-rpath-link=. tst-tlsmod10.so
LDFLAGS_tst-tls12 := -Wl,-rpath-link=. tst-tlsmod12.so
LDFLAGS_tst-tls13 := -ldl -Wl,-rpath-link=.
LDFLAGS_tst-tls14 := -ldl -Wl,-rpath-link=. tst-tlsmod14a.so
LDFLAGS_tst-tls15 := -ldl -Wl,-rpath-link=.

tst-tls3: tst-tlsmod1.so
tst-tls4: tst-tlsmod2.so
tst-tls5: tst-tlsmod2.so
tst-tls6: tst-tlsmod2.so
tst-tls7: tst-tlsmod2.so tst-tlsmod3.so
tst-tls8: tst-tlsmod2.so tst-tlsmod3.so tst-tlsmod4.so
tst-tls9: tst-tlsmod5.so tst-tlsmod6.so
tst-tls10: tst-tlsmod7.so tst-tlsmod8.so
tst-tls11: tst-tlsmod9.so tst-tlsmod10.so
tst-tls12: tst-tlsmod11.so tst-tlsmod12.so
tst-tls13: tst-tlsmod13.so tst-tlsmod13a.so
tst-tls14: tst-tlsmod14a.so tst-tlsmod14b.so
tst-tls15: tst-tlsmod15a.so tst-tlsmod15b.so

RET_tst-tls13 := 1
ifeq ($(TARGET_ARCH),mips)
RET_tst-tls15 := 1
endif

WRAPPER := env LD_LIBRARY_PATH="$$PWD:.:$(LD_LIBRARY_PATH)"
