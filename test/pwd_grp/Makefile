TESTDIR=../
include $(TESTDIR)/Rules.mak


TARGETS=test_pwd test_pwd_glibc
TARGETS+=test_grp test_grp_glibc
TARGETS+=test_pwd_diff test_grp_diff

all: $(TARGETS)

test_pwd: test_pwd.c Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(TESTCC)
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(TESTCC) $(CFLAGS) -c $< -o $@.o
	$(TESTCC) $(LDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-./$@ 2>&1 >test_pwd.out
	-@ echo " "

test_pwd_glibc: test_pwd.c Makefile
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-./$@ 2>&1 >test_pwd_glibc.out
	-@ echo " "

test_grp: test_grp.c Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(TESTCC)
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(TESTCC) $(CFLAGS) -c $< -o $@.o
	$(TESTCC) $(LDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-./$@ 2>&1 >test_grp.out
	-@ echo " "

test_grp_glibc: test_grp.c Makefile
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs GNU libc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) $@.o -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@
	-./$@ 2>&1 >test_grp_glibc.out
	-@ echo " "

test_pwd_diff: test_pwd_glibc test_pwd
	-@ echo "-------"
	-@ echo " "
	-@ echo "Diffing output: "
	-@ echo " "
	-diff -u test_pwd_glibc.out test_pwd.out
	-@ echo " "

test_grp_diff: test_grp_glibc test_grp
	-@ echo "-------"
	-@ echo " "
	-@ echo "Diffing output: "
	-@ echo " "
	-diff -u test_grp_glibc.out test_grp.out
	-@ echo " "

clean:
	rm -f *.[oa] *~ core $(TARGETS) *.out


