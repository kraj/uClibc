include $(TOPDIR)Rules.mak

LDSO_VERSION=1.9.11

# Do NOT use -fomit-frame-pointer -- It won't work!
CFLAGS	+= -DVERSION=\"$(LDSO_VERSION)\"

CC = $(TOPDIR)extra/gcc-uClibc/$(NATIVE_ARCH)-uclibc-gcc

CFLAGS += -DUCLIBC_ROOT_DIR=\"$(ROOT_DIR)\"
