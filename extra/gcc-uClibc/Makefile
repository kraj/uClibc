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

all: gcc-uClibc

clean:
	rm -f gcc-uClibc.h *-gcc-uclibc core

gcc-uClibc.h: clean
	@echo "/* this file is created by make */" > gcc-uClibc.h
	@echo "#define UCLIBC_DIR " \"$(UCLIBC_DIR)/\" >> gcc-uClibc.h
	@echo "#define GCC_BIN " \"$(GCC_BIN)\" >> gcc-uClibc.h
	@echo "#define GCC_LIB " \"$(GCC_LIB)\" >> gcc-uClibc.h
	@echo "#define GCC_INCDIR " \"-I$(GCCINCDIR)/\" >> gcc-uClibc.h
	@echo "#define TARGET_ARCH " \"$(TARGET_ARCH)\" >> gcc-uClibc.h
	@echo "#define DYNAMIC_LINKER " \"$(DYNAMIC_LINKER)\" >> gcc-uClibc.h

gcc-uClibc: gcc-uClibc.h gcc-uClibc.c
	gcc -Wall -O2 -s gcc-uClibc.c -o $(TARGET_ARCH)-uclibc-gcc

