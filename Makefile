DIRS = headers error gtermcap misc stdio2 time getent regexp string termios sysdeps \
malloc-simple net rpc

all: libc.a
# crt0.o

libc.a: $(DIRS) dummy
	$(CROSS)ranlib $@

#crt0.o: crt

headers: dummy
	if [ ! -L "include/asm" ]; then ln -s /usr/src/linux/include/asm include/asm ; fi
	if [ ! -L "include/net" ]; then ln -s /usr/src/linux/include/net include/net ; fi
	if [ ! -L "include/linux" ]; then ln -s /usr/src/linux/include/linux include/linux ; fi

error: dummy
	make -C error

gtermcap: dummy
	make -C gtermcap

misc: dummy
	make -C misc

stdio2: dummy
	make -C stdio2

time: dummy
	make -C time

getent: dummy
	make -C getent

regexp: dummy
	make -C regexp

string: dummy
	make -C string

termios: dummy
	make -C termios

sysdeps: dummy
	make -C sysdeps

malloc-simple: dummy
	make -C malloc-simple

net: dummy
	make -C net

rpc: dummy
	make -C rpc

crt: dummy
	make -C crt

dummy:

clean:
	-rm -f `find -name \*.[oa]` `find -name \*~` core
	-rm -f include/asm include/net include/linux

