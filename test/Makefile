DIRS = assert ctype silly stdlib string 

all: $(DIRS)

assert: dummy
	make -C assert

ctype: dummy
	make -C ctype

silly: dummy
	make -C silly

stdlib: dummy
	make -C stdlib

string: dummy
	make -C string

tags:
	ctags -R
	
dummy:

clean:
	make -C assert clean
	make -C ctype clean
	make -C silly clean
	make -C stdlib clean
	make -C string clean

