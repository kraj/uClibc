# Makefile for uClibc
#
# Copyright (C) 2002 Erik Andersen <andersen@codepoet.org>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Library General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for more
# details.
#
# You should have received a copy of the GNU Library General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

TOPDIR=../../
include $(TOPDIR)Rules.mak


all: conf mconf

NATIVE_CFLAGS=-Wall -g -O0


CONF_SRC  =conf.c zconf.tab.c
MCONF_SRC =mconf.c zconf.tab.c
CONF_OBJS =$(patsubst %.c,%.o, $(CONF_SRC))
MCONF_OBJS=$(patsubst %.c,%.o, $(MCONF_SRC))

conf: $(CONF_OBJS) 
	$(NATIVE_CC) $(NATIVE_CFLAGS) $(NATIVE_LDFLAGS) $^ -o $@

mconf: $(MCONF_OBJS)
	$(NATIVE_CC) $(NATIVE_CFLAGS) $(NATIVE_LDFLAGS) $^ -o $@

lkc_deps:=lkc.h lkc_proto.h lkc_defs.h expr.h zconf.tab.h

conf.o: conf.c $(lkc_deps)

mconf.o: mconf.c $(lkc_deps)

zconf.tab.o: zconf.tab.c lex.zconf.c confdata.c expr.c symbol.c menu.c $(lkc_deps)

lex.zconf.o: lex.zconf.c $(lkc_deps)

%.o : %.c
	$(NATIVE_CC) $(NATIVE_CFLAGS) -I. -c $< -o $@

lkc_defs.h: lkc_proto.h
	@sed < $< > $@ 's/P(\([^,]*\),.*/#define \1 (\*\1_p)/'

###
# The following requires flex/bison
# By default we use the _shipped versions, uncomment the 
# following line if you are modifying the flex/bison src.
#LKC_GENPARSER := 1

ifdef LKC_GENPARSER

%.tab.c %.tab.h: %.y
	bison -t -d -v -b $* -p $(notdir $*) $<

lex.%.c: %.l
	flex -P$(notdir $*) -o$@ $<
else

lex.zconf.c: lex.zconf.c_shipped
	cp lex.zconf.c_shipped lex.zconf.c

zconf.tab.c: zconf.tab.c_shipped
	cp zconf.tab.c_shipped zconf.tab.c

zconf.tab.h: zconf.tab.h_shipped
	cp zconf.tab.h_shipped zconf.tab.h
endif

clean:
	rm -f *.o *~ core $(TARGETS) $(MCONF_OBJS) $(CONF_OBJS) \
		conf mconf zconf.tab.c zconf.tab.h lex.zconf.c lkc_defs.h
	$(MAKE) -C lxdialog clean

