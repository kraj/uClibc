# Makefile for uClibc
#
# Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
#
# Licensed under the GNU Library General Public License version 2 or later.
# See the COPYING.LIB file in the toplevel for more information.

TOPDIR=../../
include $(TOPDIR)Rules.mak

DIRS=

ifeq ($(UCLIBC_HAS_STRING_GENERIC_OPT),y)
DIRS += generic
endif

ifeq ($(UCLIBC_HAS_STRING_ARCH_OPT),y)
ifeq ($(TARGET_ARCH),$(wildcard $(TARGET_ARCH)))
DIRS += $(TARGET_ARCH)
endif
endif

ALL_SUBDIRS = generic arm frv i386 mips powerpc sh64 sparc x86_64

MSRC= wstring.c
MOBJ=  basename.o bcopy.o bzero.o dirname.o ffs.o memccpy.o memchr.o memcmp.o \
	memcpy.o memmove.o mempcpy.o memrchr.o 	memset.o rawmemchr.o stpcpy.o \
	stpncpy.o strcasecmp.o strcasestr.o strcat.o strchrnul.o strchr.o \
	strcmp.o strcpy.o strcspn.o strdup.o strlen.o strncasecmp.o strncat.o \
	strncmp.o strncpy.o strndup.o strnlen.o strpbrk.o strrchr.o strsep.o \
	strspn.o strstr.o strtok.o strtok_r.o strerror.o __xpg_strerror_r.o \
	_string_syserrmsgs.o __glibc_strerror_r.o \
	_string_syssigmsgs.o sys_siglist.o strsignal.o psignal.o \
	 __xpg_basename.o strlcat.o strlcpy.o sys_errlist.o memmem.o

MOBJW= wcscasecmp.o wcscat.o wcschrnul.o wcschr.o wcscmp.o wcscpy.o wcscspn.o \
	wcsdup.o wcslen.o wcsncasecmp.o wcsncat.o wcsncmp.o wcsncpy.o \
	wcsnlen.o wcspbrk.o wcsrchr.o wcsspn.o wcsstr.o wcstok.o wmemchr.o \
	wmemcmp.o wmemcpy.o wmemmove.o wmempcpy.o wmemset.o wcpcpy.o wcpncpy.o \
	__wcslcpy.o \
	wcsxfrm.o strxfrm.o # wcscoll strcoll.o

MOBJx=
MOBJWx=

ifeq ($(UCLIBC_HAS_XLOCALE),y)
	MOBJx += strcasecmp_l.o strncasecmp_l.o
	MOBJWx += wcscasecmp_l.o wcsncasecmp_l.o wcsxfrm_l.o strxfrm_l.o
endif

#ffsl ffsll

OBJS=$(MOBJ) $(MOBJx)

ifeq ($(UCLIBC_HAS_WCHAR),y)
	OBJS += $(MOBJW) $(MOBJWx)
endif

OBJ_LIST=../obj.string

all: $(OBJ_LIST) subdirs

$(OBJ_LIST): $(OBJS)
	echo $(patsubst %, string/%, $(OBJS)) > $(OBJ_LIST)

$(MOBJ): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJx): $(MSRC)
	$(CC) $(CFLAGS) -DL_$* -D__UCLIBC_DO_XLOCALE $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJW): $(MSRC)
	$(CC) $(CFLAGS) -DWANT_WIDE -DL_$* $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(MOBJWx): $(MSRC)
	$(CC) $(CFLAGS) -DWANT_WIDE -DL_$* -D__UCLIBC_DO_XLOCALE $< -c -o $*.o
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

clean: subdirs_clean
	$(RM) *.[oa] *~ core

subdirs: $(patsubst %, _dir_%, $(DIRS))
subdirs_clean: $(patsubst %, _dirclean_%, $(ALL_SUBDIRS))

$(patsubst %, _dir_%, $(DIRS)) : $(OBJS)
	$(MAKE) -C $(patsubst _dir_%, %, $@)

$(patsubst %, _dirclean_%, $(ALL_SUBDIRS)) : dummy
	$(MAKE) -C $(patsubst _dirclean_%, %, $@) clean

.PHONY: dummy
