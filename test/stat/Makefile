TESTDIR=../
include $(TESTDIR)/Rules.mak



TARGETS=stat stat_glibc

all: $(TARGETS)

stat_source:
	-@ rm -f $(TARGETS)
	-@ echo "-------"
	-@ echo "stat.c source: "
	-@ echo " "
	-@ cat stat.c
	-@ echo " "

stat: stat.c Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(CC)
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-$(LDD) $@
	ls -l $@
	-./$@
	-@ echo " "

stat_glibc: stat.c Makefile
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(HOST_CC) $(GLIBC_CFLAGS) -c $< -o $@.o
	$(HOST_CC) $(GLIBC_LDFLAGS) $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-$(LDD) $@
	ls -l $@
	-./$@
	-@ echo " "

clean:
	rm -f *.[oa] *~ core $(TARGETS)


