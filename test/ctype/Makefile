TESTDIR=../
include $(TESTDIR)/Rules.mak

TARGETS=ctype
all: $(TARGETS)

ctype: ctype.c ../testsuite.h Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(CC)
	-@ echo "-------"
	-@ echo " "
	-@ echo "Compiling vs uClibc: "
	-@ echo " "
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) $(LDFLAGS) $@.o -o $@ $(EXTRA_LIBS)
	$(STRIPTOOL) -x -R .note -R .comment $@
	./$@
	-@ echo " "
clean:
	rm -f *.[oa] *~ core $(TARGETS)


