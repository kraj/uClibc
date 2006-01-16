/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <features.h>
#include <asm/unistd.h>

.text
.global _vfork
.type	_vfork,STT_FUNC;
.align 4
_vfork:
	p0 = __NR_vfork;
	excpt 0;
	rts;
.size _vfork,.-_vfork
