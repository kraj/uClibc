#
# These names are used for NPTL since we have to build seperate archives
# for the static and shared objects.
#
LIB_NAME	:= libpthread
AR_LIB_NAME	:= $(TOPDIR)lib/$(LIB_NAME).a
AR_SO_LIB_NAME	:= $(TOPDIR)lib/shared_$(LIB_NAME).a
SO_LIB_NAME	:= $(TOPDIR)lib/$(LIB_NAME).so
SO_FULL_NAME	:= $(LIB_NAME)-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

CFLAGS-LIBC	:= $(filter-out -O0 -O1 -Os, $(CFLAGS)) $(PTINC) -O2 \
		   -I$(TOPDIR)libpthread/nptl \
		   -I$(TOPDIR)ldso/include -std=gnu99
ASFLAGS-LIBC	:= $(filter-out -std=gnu99, $(CFLAGS-LIBC)) -D__ASSEMBLER__
CFLAGS-LIBP	:= $(CFLAGS-LIBC) -DNOT_IN_libc=1 -DIS_IN_libpthread=1
ASFLAGS-LIBP	:= $(filter-out -std=gnu99, $(CFLAGS-LIBP)) -D__ASSEMBLER__
