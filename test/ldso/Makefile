TESTDIR=../
include $(TESTDIR)/Rules.mak

all: dltest shared run

dltest:
	$(CC) $(CFLAGS) -c dltest.c -o dltest.o
	$(CC) $(CFLAGS) -c howdy.c -o howdy.o

shared:
	$(CC) $(CFLAGS) -shared -o libhowdy.so -Wl,-soname,libhowdy.so howdy.o
	$(CC) $(CFLAGS) -o dltest dltest.o -ldl
	
run:
	@echo Running dltest
	./dltest

clean:
	rm -f *.o *.so dltest core libhowdy.so
