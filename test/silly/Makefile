TESTDIR=../
include $(TESTDIR)/Rules.mak



TARGETS=hello_source hello hello_glibc

all: $(TARGETS)

hello_source:
	-@ rm -f $(TARGETS)
	-@ echo "-------"
	-@ echo "hello.c source: "
	-@ echo " "
	-@ cat hello.c
	-@ echo " "

hello: hello.c Makefile $(TESTDIR)/Config $(TESTDIR)/Rules.mak $(TESTCC)
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

hello_glibc: hello.c Makefile
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


