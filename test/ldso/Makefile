TESTDIR=../
include $(TESTDIR)/Rules.mak

all: dltest shared run

dltest:
	$(TESTCC) $(TEST_CFLAGS) -c dltest.c -o dltest.o
	$(TESTCC) $(TEST_CFLAGS) -c howdy.c -o howdy.o

shared:
	$(TESTCC) $(TEST_CFLAGS) -shared -o libhowdy.so -Wl,-soname,libhowdy.so howdy.o
	$(TESTCC) $(TEST_CFLAGS) -o dltest dltest.o -ldl
	
run:
	@echo Running dltest
	./dltest

clean:
	rm -f *.o *.so dltest core

