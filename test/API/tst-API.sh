#!/bin/sh

std="$1"
shift
cod="$*"

result=0

for l in $top_builddir/lib/lib*-*.so; do \
	readelf -D -W -s $l | \
	egrep -v "(UND|LOCAL|^Symbol table for image:|^[[:space:]]*Num[[:space:]])" | \
	$AWK '{print $NF}'; \
	done | sort | uniq > $uclibc_out
for code in $cod; do cat $code.$std.syms; done | sort | uniq > $glibc_out
result=0
exit $result
