TOPDIR = ../../
include $(TOPDIR)Rules.mak

# NOTE: This may need to be modified for your system
DYNAMIC_LINKER = $(INSTALL_DIR)/lib/ld-linux-uclibc.so.$(MAJOR_VERSION)

UCLIBC_DIR = $(shell (cd ../.. ; /bin/pwd))
GCC_BIN = $(CC)
GCC_LIB = $(shell $(CC) -print-libgcc-file-name )
#GCCINCDIR inherited from Rules.mak

all: gcc-uClibc

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

install:
	@if [ -f $(TARGET_ARCH)-uclibc-gcc ] ; then \
		mkdir -p $(INSTALL_DIR)/usr/bin ; \
		install -m 755 $(TARGET_ARCH)-uclibc-gcc $(INSTALL_DIR)/usr/bin/ ; \
	fi
	$(shell (for app in addr2line ar as gasp ld nm objcopy objdump \
						ranlib size strings strip; do \
		    ln -fs `which $(CROSS)$${app}` $(INSTALL_DIR)/bin/$${app}; \
		    done)\
	)
	$(shell (for app in cc1 cc1plus; do \
		    ln -fs $(CC1DIR)$${app} $(INSTALL_DIR)/bin/$${app}; \
		    done)\
	)

clean:
	rm -f gcc-uClibc.h *-uclibc-gcc core


