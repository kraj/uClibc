TOPDIR=../
include $(TOPDIR)Rules.make

LIBC=../libc.a

OBJ=rx.o

all: $(LIBC)

$(LIBC): $(LIBC)($(OBJ))

$(LIBC)(rx.o):	rx.c
	$(CC) $(CFLAGS) -DL_$* $< -c -o $*.o
	$(AR) $(ARFLAGS) $@ $*.o

clean:
	rm -f libc.a *.o core mon.out timer.t.h dMakefile dtr try timer
