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

stat: stat.c Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(TESTCC)
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(TESTCC) $(CFLAGS) -c $< -o $@.o
	$(TESTCC) $(LDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-ldd $@
	ls $(LSFLAGS) $@
	-./$@
	-@ echo " "

stat_glibc: stat.c Makefile
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-ldd $@
	ls $(LSFLAGS) $@
	-./$@
	-@ echo " "

clean:
	rm -f *.[oa] *~ core $(TARGETS)


