#!/bin/sh

ret=0

# Make sure nothing uses the ARCH_HAS_MMU option anymore
result=$(
grep -rsHI \
	__ARCH_HAS_MMU__ ../.. \
	| grep -v \
		-e include/bits/uClibc_config.h \
		-e test/build/check_config_options.sh \
		-e /.svn/
)
if test -n "$result" ; then
	echo "The build system is incorrectly using ARCH_HAS_MMU:"
	echo "$result"
	ret=1
fi

exit $ret
