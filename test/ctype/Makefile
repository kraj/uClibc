TESTDIR=../
include $(TESTDIR)/Rules.mak

TARGETS=ctype ctype_run
all: $(TARGETS)

ctype: ctype.c ../testsuite.h Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(CC)
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@

ctype_run:
	./ctype
	-@ echo " "
clean:
	rm -f *.[oa] *~ core $(TARGETS)


