TESTDIR=../
include $(TESTDIR)/Rules.mak


TARGETS=assert
all: $(TARGETS)

assert: assert.c Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(TESTCC)
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(TESTCC) $(CFLAGS) -c $< -o $@.o
	$(TESTCC) $(CFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	-./$@
	-@ echo " "

clean:
	rm -f *.[oa] *~ core $(TARGETS)


