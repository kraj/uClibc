TESTDIR=../
include $(TESTDIR)/Rules.mak

all: dltest libhowdy.so run

dltest.o: dltest.c
	$(CC) $(CFLAGS) -c dltest.c -o dltest.o

howdy.o: howdy.c
	$(CC) $(CFLAGS) -fPIC -c howdy.c -o howdy.o

libhowdy.so: howdy.o
	$(CC) $(CFLAGS) -shared -o libhowdy.so -Wl,-soname,libhowdy.so howdy.o

dltest: dltest.o
	$(CC) $(CFLAGS) -o dltest dltest.o -ldl
	
run: dltest libhowdy.so
	@echo Running dltest
	./dltest

clean:
	rm -f *.o *.so dltest core libhowdy.so
