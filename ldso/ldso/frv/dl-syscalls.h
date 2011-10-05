/* Copyright (C) 2003, 2004 Red Hat, Inc.
 * Contributed by Alexandre Oliva <aoliva@redhat.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#ifdef __NR_pread
#define __NR___syscall_pread __NR_pread
static __always_inline _syscall5(ssize_t, __syscall_pread, int, fd, void *, buf,
			size_t, count, off_t, offset_hi, off_t, offset_lo);

static __always_inline ssize_t
_dl_pread(int fd, void *buf, size_t count, off_t offset)
{
  return(__syscall_pread(fd,buf,count,__LONG_LONG_PAIR (offset >> 31, offset)));
}
#endif
