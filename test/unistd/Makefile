TESTDIR=../
include $(TESTDIR)/Rules.mak



TARGETS=fork fork_glibc
all: $(TARGETS)

fork: fork.c ../testsuite.h Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(CC)
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	./$@
	-@ echo " "

fork_glibc: fork.c ../testsuite.h Makefile
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(HOST_CC) $(GLIBC_CFLAGS) -c $< -o $@.o
	$(HOST_CC) $(GLIBC_LDFLAGS) $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-./$@
	-@ echo " "

clean:
	rm -f *.[oa] *~ core $(TARGETS)


