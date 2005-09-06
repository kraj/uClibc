#
# These names are used for NPTL since we have to build seperate archives
# for the static and shared objects. We export them to sub-makes.
#
LIBPTHREAD_STATIC_ARCHIVE := libpthread.a
LIBPTHREAD_SHARED_ARCHIVE := libpthread-shared.a

CFLAGS-LIBC	:= $(filter-out -O0 -O1 -Os, $(CFLAGS)) -O2 -I$(PTDIR)	\
		   -I$(TOPDIR)ldso/include -std=gnu99
ASFLAGS-LIBC	:= $(filter-out -std=gnu99, $(CFLAGS-LIBC)) -D__ASSEMBLER__

CFLAGS-LIBP	:= $(CFLAGS-LIBC) -DNOT_IN_libc=1 -DIS_IN_libpthread=1
ASFLAGS-LIBP	:= $(filter-out -std=gnu99, $(CFLAGS-LIBP)) -D__ASSEMBLER__
