#!/bin/sh

# Make sure nothing uses the ARCH_HAS_MMU option anymore
ret=$(
grep -rHI \
	__ARCH_HAS_MMU__ ../.. \
	| grep -v \
		-e include/bits/uClibc_config.h \
		-e test/build/check_config_options.sh
)
if test -n "$ret" ; then
	echo "The build system is incorrectly using ARCH_HAS_MMU:"
	echo "$ret"
fi

exit 0
