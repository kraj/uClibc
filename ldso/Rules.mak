include $(TOPDIR)Rules.mak

LDSO_VERSION=1.9.11

DEVEL=true

# Do NOT use -fomit-frame-pointer -- It won't work!
CFLAGS	+= -DVERSION=\"$(LDSO_VERSION)\"

CC = $(TOPDIR)extra/gcc-uClibc/$(NATIVE_ARCH)-uclibc-gcc
ifeq ($(DEVEL),true)
	CFLAGS += -DUCLIBC_INSTALL_DIR=\"$(INSTALL_DIR)\"
endif
