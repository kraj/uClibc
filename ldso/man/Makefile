include ../Config.mk

ALL = #ld.so.info

all:	$(ALL)

ld.so.info: ld.so.texi
	makeinfo $<

clean:
	rm -f $(ALL) *~
