include Config.mk

SUBDIRS = util d-link # man

all:
	set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d ; done

install: all
	sh instldso.sh

clean:
	set -e ; for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@ ; done
	-find . -name '*~' | xargs rm -f
