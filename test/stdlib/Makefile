TESTDIR=../
include $(TESTDIR)/Rules.mak



TARGETS=testmalloc testmalloc_glibc 
TARGETS+=mallocbug mallocbug_glibc
TARGETS+=teststrtol teststrtol_glibc teststrtol_diff
TARGETS+=qsort qsort_glibc qsort_diff

all: $(TARGETS)

testmalloc_source:
	-@ echo "-------"
	-@ echo "testmalloc.c source: "
	-@ echo " "
	-@ cat testmalloc.c
	-@ echo " "

testmalloc: testmalloc.c Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(CC)
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

testmalloc_glibc: testmalloc.c Makefile
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

mallocbug: mallocbug.c Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(CC)
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

mallocbug_glibc: mallocbug.c Makefile
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

teststrtol_source:
	-@ echo "-------"
	-@ echo "teststrtol.c source: "
	-@ echo " "
	-@ cat teststrtol.c
	-@ echo " "

teststrtol: teststrtol.c Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(CC)
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-$(LDD) $@
	ls -sh $@
	-./$@ > $@.out
	-@ echo " "

teststrtol_glibc: teststrtol.c Makefile
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(HOST_CC) $(GLIBC_CFLAGS) -c $< -o $@.o
	$(HOST_CC) $(GLIBC_LDFLAGS) $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-$(LDD) $@
	ls -sh $@
	-./$@ > $@.out
	-@ echo " "

teststrtol_diff: teststrtol_glibc teststrtol
	-@ echo "-------"
	-@ echo " "
	-@ echo "Diffing output: "
	-@ echo " "
	-diff -u teststrtol_glibc.out teststrtol.out
	-@ echo " "

qsort: qsort.c Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(CC)
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-$(LDD) $@
	ls -l $@
	-./$@ > $@.out
	-@ echo " "

qsort_glibc: qsort.c Makefile
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(HOST_CC) $(GLIBC_CFLAGS) -c $< -o $@.o
	$(HOST_CC) $(GLIBC_LDFLAGS) $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-$(LDD) $@
	ls -sh $@
	-./$@ > $@.out
	-@ echo " "

qsort_diff: qsort_glibc qsort
	-@ echo "-------"
	-@ echo " "
	-@ echo "Diffing output: "
	-@ echo " "
	-diff -u qsort_glibc.out qsort.out
	-@ echo " "

clean:
	rm -f *.[oa] *~ core $(TARGETS) teststrtol_glibc.out teststrtol.out


