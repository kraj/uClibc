/*
 * remap_file_pages() for uClibc
 *
 * Copyright (C) 2008 Will Newton <will.newton@imgtec.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>

#ifdef __NR_remap_file_pages

_syscall5(int, remap_file_pages, unsigned long, start, unsigned long, size,
	  unsigned long, prot, unsigned long, pgoff, unsigned long, flags);

#endif
