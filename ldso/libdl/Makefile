TOPDIR=../../../
include $(TOPDIR)Rules.mak
include $(TOPDIR)/ld.so-1/Config.mk

CFLAGS += -DNO_UNDERSCORE -DVERBOSE_DLINKER -DUSE_CACHE
CFLAGS += #-fPIC -D__PIC__ #-funroll-loops

LIBDL = libdl.so

CSRC= dlib.c
COBJS=$(patsubst %.c,%.o, $(CSRC))
OBJS=$(COBJS)

$(COBJS): %.o : %.c
	$(CC) -I.. -I../$(TARGET_ARCH) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

ELF_LDFLAGS=--shared -nostartfiles -nostdlib # using GNU ld
#ELF_LDFLAGS=-G  # with SVr4 ld

lib:: $(OBJS)
	$(LD) $(ELF_LDFLAGS) -o $(LIBDL).$(LDSO_VMAJOR) \
		 -soname $(LIBDL).$(LDSO_VMAJOR) *.o -lc

obj: $(OBJS)

realclean::
	$(RM) -f .depend $(LIBDL) core *.o *.a *.s *.i tmp_make foo *~

clean::
	$(RM) -f $(LIBDL) core *.o *.a *.s *.i tmp_make foo *~
