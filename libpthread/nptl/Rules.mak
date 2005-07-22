#
# These names are used for NPTL since we have to build seperate archives
# for the static and shared objects. We export them to sub-makes.
#
LIBPTHREAD_STATIC_ARCHIVE := libpthread.a
LIBPTHREAD_SHARED_ARCHIVE := libpthread-shared.a

CFLAGS-NPTL := $(CFLAGS:-O0=-O2) \
	       -I$(PTDIR) \
	       -I$(TOPDIR)ldso/include \
	       -DNOT_IN_libc=1 -DIS_IN_libpthread=1 -std=gnu99

ASFLAGS-NPTL := $(filter-out -std=gnu99, $(CFLAGS-NPTL)) -D__ASSEMBLER__
