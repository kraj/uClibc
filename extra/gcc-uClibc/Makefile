TOPDIR = ../../
include $(TOPDIR)Rules.mak

# NOTE: This may need to be modified for your system
DYNAMIC_LINKER = /usr/$(TARGET_ARCH)-linux-uclibc/lib/ld-linux-uclibc.so.1

UCLIBC_DIR = $(shell (cd ../.. ; /bin/pwd))
GCC_BIN = $(CC)
GCC_LIB = $(shell $(CC) -print-libgcc-file-name )
#GCCINCDIR inherited from Rules.mak

all: gcc-uClibc

clean:
	rm -f gcc-uClibc.h *-uclibc-gcc core

gcc-uClibc.h: clean
	@echo "/* this file is created by make */" > gcc-uClibc.h
	@echo "#define UCLIBC_INSTALL_DIR " \"$(INSTALL_DIR)/\" >> gcc-uClibc.h
	@echo "#define UCLIBC_BUILD_DIR " \"$(UCLIBC_DIR)/\" >> gcc-uClibc.h
	@echo "#define GCC_BIN " \"$(GCC_BIN)\" >> gcc-uClibc.h
	@echo "#define GCC_LIB " \"$(GCC_LIB)\" >> gcc-uClibc.h
	@echo "#define GCC_INCDIR " \"-I$(GCCINCDIR)/\" >> gcc-uClibc.h
	@echo "#define TARGET_ARCH " \"$(TARGET_ARCH)\" >> gcc-uClibc.h
	@echo "#define DYNAMIC_LINKER " \"$(DYNAMIC_LINKER)\" >> gcc-uClibc.h

gcc-uClibc: gcc-uClibc.h gcc-uClibc.c
	gcc -Wall -O2 -s gcc-uClibc.c -o $(TARGET_ARCH)-uclibc-gcc

