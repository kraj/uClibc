# Figure out what arch to build...

ARCH = $(shell uname -m | sed -e 's/i.86/i386/' | sed -e 's/sparc.*/sparc/')

all: $(ARCH) common

$(ARCH): dummy
	@echo Building for ARCH=$(ARCH)
	make -C $(ARCH)

common:	dummy
	echo Building common stuff
	make -C common

clean:
	rm -f *.o
	make -C common clean
	make -C $(ARCH) clean

.PHONY: dummy
