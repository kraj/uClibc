TOPDIR=../../
include $(TOPDIR)/ld.so-1/Rules.mak

CFLAGS += -DUSE_CACHE -fPIC -D__PIC__ #-funroll-loops

CSRC= boot1.c hash.c readelflib1.c vsprintf.c $(TARGET_ARCH)/elfinterp.c
COBJS=$(patsubst %.c,%.o, $(CSRC))
ASRC=$(shell ls $(TARGET_ARCH)/*.S)
AOBJS=$(patsubst %.S,%.o, $(ASRC))
OBJS=$(AOBJS) $(COBJS)

ELF_LDFLAGS=--shared # using GNU ld

all: lib

lib:: $(OBJS) $(DLINK_OBJS)
	$(LD) -e _dl_boot $(ELF_LDFLAGS) -o $(DLINKER).$(LDSO_VMAJOR) \
	  -soname $(DLINKER).$(LDSO_VMAJOR) $(OBJS)

$(COBJS): %.o : %.c
	$(CC) -I. -I./$(TARGET_ARCH) -I../libdl $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

realclean::
	$(RM) -f .depend $(DLINKER) core *.o *.a *.s *.i tmp_make foo *~

clean::
	$(RM) -f $(DLINKER)* $(OBJS) core *.o *.a *.s *.i tmp_make foo *~

