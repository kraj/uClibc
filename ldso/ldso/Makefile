TOPDIR=../../
include $(TOPDIR)/ld.so-1/Rules.mak

CFLAGS += -DUSE_CACHE -fPIC -D__PIC__ #-DDL_DEBUG #-funroll-loops

CSRC= boot1.c hash.c readelflib1.c vsprintf.c $(TARGET_ARCH)/elfinterp.c
COBJS=$(patsubst %.c,%.o, $(CSRC))
ASRC=$(shell ls $(TARGET_ARCH)/*.S)
AOBJS=$(patsubst %.S,%.o, $(ASRC))
OBJS=$(AOBJS) $(COBJS)

ELF_LDFLAGS=--shared # using GNU ld

all: ld.so.h lib

ld.so.h:
	echo "char *_dl_progname = \""$(UCLIBC_LDSO)"\";" > ld.so.h

lib:: $(OBJS) $(DLINK_OBJS)
	$(LD) -e _dl_boot $(ELF_LDFLAGS) -o $(UCLIBC_LDSO) \
		-soname $(UCLIBC_LDSO) $(OBJS)

$(COBJS): %.o : %.c
	$(CC) -I. -I./$(TARGET_ARCH) -I../libdl $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

realclean::
	$(RM) -f .depend $(UCLIBC_LDSO)* core *.o *.a *.s *.i tmp_make foo *~

clean::
	$(RM) -f $(UCLIBC_LDSO)* $(OBJS) core *.o *.a *.s *.i tmp_make foo *~

