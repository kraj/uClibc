TOPDIR=../../
include $(TOPDIR)Rules.mak
include $(TOPDIR)/ld.so-1/Config.mk

DIRS = $(TARGET_ARCH) libdl

CFLAGS += -DNO_UNDERSCORE -DVERBOSE_DLINKER
CFLAGS += -DUSE_CACHE #-fPIC -D__PIC__ #-funroll-loops

CSRC= boot1.c hash.c readelflib1.c vsprintf.c
COBJS=$(patsubst %.c,%.o, $(CSRC))
OBJS=$(COBJS)

DLINKER = ld-linux-uclibc.so

ELF_LDFLAGS=--shared # using GNU ld

ifneq ($(DIRS),)
lib realclean clean::
	@set -e; for i in $(DIRS); do \
	  echo making $@ in $$i; \
	  $(MAKE) -C $$i $@; \
	done;
endif

lib:: $(OBJS)
	$(LD) -e _dl_boot $(ELF_LDFLAGS) -o $(DLINKER).$(LDSO_VMAJOR) \
	  -soname $(DLINKER).$(LDSO_VMAJOR) *.o

$(COBJS): %.o : %.c
	$(CC) -I. -I./$(TARGET_ARCH) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

realclean::
	$(RM) -f .depend $(DLINKER) core *.o *.a *.s *.i tmp_make foo *~

clean::
	$(RM) -f $(DLINKER) core *.o *.a *.s *.i tmp_make foo *~

