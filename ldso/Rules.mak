include $(TOPDIR)Rules.mak

# Do NOT use -fomit-frame-pointer -- It won't work!
CFLAGS += -DUCLIBC_ROOT_DIR=\"$(ROOT_DIR)\"
CC = $(TOPDIR)extra/gcc-uClibc/$(TARGET_ARCH)-uclibc-gcc
