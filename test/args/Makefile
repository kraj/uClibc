TESTDIR=../
include $(TESTDIR)/Rules.mak


TARGETS=arg_test
all: $(TARGETS)

arg_test: arg_test.c Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(TESTCC)
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(TESTCC) $(TEST_CFLAGS) -c $< -o $@.o
	$(TESTCC) $(LDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-./$@ a b c d e f g h
	-@ echo " "

clean:
	rm -f *.[oa] *~ core $(TARGETS)


