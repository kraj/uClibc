
TOPDIR = ../../
include $(TOPDIR)Rules.mak

# NOTE: This may need to be modified on each system.
ifeq ($(TARGET_ARCH), i386)
# Right now, I'm using .1 since .2 pulls in libc6 on my system. Manuel
DYNAMIC_LINKER = /lib/ld-linux.so.1
else
# For Erik's arm case.
DYNAMIC_LINKER = $(shell ls /lib/ld*.so.[0-9] | sort | tail -n 1)
endif

UCLIBC_DIR = $(shell (cd ../.. ; /bin/pwd))
GCC_BIN = $(CC)
GCC_LIB = $(shell $(CC) -print-libgcc-file-name )
#GCCINCDIR inherited from Rules.mak

NATIVE_ARCH = $(shell uname -m | sed -e 's/i.86/i386/' -e 's/sparc.*/sparc/' -e 's/arm.*/arm/g' -e 's/m68k.*/m68k/')
GCC_UCLIBC = gcc-uClibc-cross
ifeq ($(TARGET_ARCH), $(NATIVE_ARCH))
	GCC_UCLIBC = gcc-uClibc-native
endif

all: $(GCC_UCLIBC)

clean:
	rm -f gcc-uClibc.h gcc-uClibc-* core

gcc-uClibc.h: clean
	@echo "/* this file is created by make */" > gcc-uClibc.h
	@echo "#define UCLIBC_DIR " \"$(UCLIBC_DIR)/\" >> gcc-uClibc.h
	@echo "#define GCC_BIN " \"$(GCC_BIN)\" >> gcc-uClibc.h
	@echo "#define GCC_LIB " \"$(GCC_LIB)\" >> gcc-uClibc.h
	@echo "#define GCC_INCDIR " \"-I$(GCCINCDIR)/\" >> gcc-uClibc.h
	@echo "#define TARGET_ARCH " \"$(TARGET_ARCH)\" >> gcc-uClibc.h
	@echo "#define DYNAMIC_LINKER " \"$(DYNAMIC_LINKER)\" >> gcc-uClibc.h

# uClibc built for native environment, so why not use it ;-)
# Use the system libc instead of uClibc, to facilitate bootstrapping
# new architectures.
#gcc-uClibc-native: gcc-uClibc.h gcc-uClibc.c
#	$(CC) $(CFLAGS) -nostdinc -I$(UCLIBC_DIR)/include -I$(GCC_INC) \
#		-Wl,-static gcc-uClibc.c \
#		$(UCLIBC_DIR)/crt0.o \
#		-nostdlib $(GCC_LIB) $(UCLIBC_DIR)/libc.a \
#		-s -o gcc-uClibc-$(TARGET_ARCH) #-DDEBUG

gcc-uClibc-native: gcc-uClibc.h gcc-uClibc.c
#	don't use CFLAGS since may not be appropriate
	gcc -s gcc-uClibc.c -o gcc-uClibc-$(TARGET_ARCH)

gcc-uClibc-cross: gcc-uClibc.h gcc-uClibc.c
#	don't use CFLAGS since may not be appropriate
	gcc -s gcc-uClibc.c -o gcc-uClibc-$(TARGET_ARCH)

