TOPDIR=../../
include $(TOPDIR)Rules.mak
include $(TOPDIR)/ld.so-1/Config.mk

CFLAGS += -DNO_UNDERSCORE -DVERBOSE_DLINKER -DUSE_CACHE
CFLAGS += #-fPIC -D__PIC__ #-funroll-loops

CSRC= dlib.c
COBJS=$(patsubst %.c,%.o, $(CSRC))
OBJS=$(COBJS)

all: lib

$(COBJS): %.o : %.c
	$(CC) -I../d-link -I../d-link/$(TARGET_ARCH) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

ELF_LDFLAGS=--shared -nostartfiles -nostdlib # using GNU ld
#ELF_LDFLAGS=-G  # with SVr4 ld

lib:: $(OBJS)
	$(CC) $(ELF_LDFLAGS) \
		-o $(LIBDL).$(LDSO_VMAJOR) -Wl,-soname -Wl,$(LIBDL).$(LDSO_VMAJOR) \
		*.o $(TOPDIR)uClibc-0.95.so

obj: $(OBJS)

realclean::
	$(RM) -f .depend $(LIBDL) core *.o *.a *.s *.i tmp_make foo *~

clean::
	$(RM) -f $(LIBDL)* core *.o *.a *.s *.i tmp_make foo *~

