DIRS = headers error getent malloc-simple misc net regexp stdio2 \
	    string sysdeps termios time #rpc

all: libc.a

libc.a: $(DIRS) dummy
	@echo
	@echo Finally finished compiling...
	@echo
	$(CROSS)ranlib $@

headers: dummy
	if [ ! -L "include/asm" ]; then ln -s /usr/src/linux/include/asm include/asm ; fi
	if [ ! -L "include/net" ]; then ln -s /usr/src/linux/include/net include/net ; fi
	if [ ! -L "include/linux" ]; then ln -s /usr/src/linux/include/linux include/linux ; fi

error: dummy
	make -C error

getent: dummy
	make -C getent

malloc-simple: dummy
	make -C malloc-simple

misc: dummy
	make -C misc

net: dummy
	make -C net

regexp: dummy
	make -C regexp

rpc: dummy
	make -C rpc

stdio2: dummy
	make -C stdio2

string: dummy
	make -C string

sysdeps: dummy
	make -C sysdeps

termios: dummy
	make -C termios

time: dummy
	make -C time

dummy:

clean:
	-rm -f `find -name \*.[oa]` `find -name \*~` core
	-rm -rf include/asm include/net include/linux

