TOPDIR=../../../
include $(TOPDIR)Rules.mak

HOSTCFLAGS += -DLOCALE 
LIBS = -lncurses

ifeq (/usr/include/ncurses/ncurses.h, $(wildcard /usr/include/ncurses/ncurses.h))
	NATIVE_CFLAGS += -I/usr/include/ncurses -DCURSES_LOC="<ncurses.h>"
else
ifeq (/usr/include/ncurses/curses.h, $(wildcard /usr/include/ncurses/curses.h))
	NATIVE_CFLAGS += -I/usr/include/ncurses -DCURSES_LOC="<ncurses/curses.h>"
else
ifeq (/usr/include/ncurses.h, $(wildcard /usr/include/ncurses.h))
	NATIVE_CFLAGS += -DCURSES_LOC="<ncurses.h>"
else
	NATIVE_CFLAGS += -DCURSES_LOC="<curses.h>"
endif
endif
endif


OBJS = checklist.o menubox.o textbox.o yesno.o inputbox.o \
       util.o lxdialog.o msgbox.o

%.o: %.c
	$(NATIVE_CC) $(NATIVE_CFLAGS) -c -o $@ $<

all: ncurses lxdialog

lxdialog: $(OBJS)
	$(NATIVE_CC) -o lxdialog $(OBJS) $(LIBS)

.PHONY: ncurses

ncurses:
	@echo "main() {}" > lxtemp.c
	@if $(NATIVE_CC) lxtemp.c $(LIBS) ; then \
		rm -f lxtemp.c a.out; \
	else \
		rm -f lxtemp.c; \
		echo -e "\007" ;\
		echo ">> Unable to find the Ncurses libraries." ;\
		echo ">>" ;\
		echo ">> You must have Ncurses installed in order" ;\
		echo ">> to use 'make menuconfig'" ;\
		echo ;\
		exit 1 ;\
	fi

clean:
	rm -f core *.o *~ lxdialog
