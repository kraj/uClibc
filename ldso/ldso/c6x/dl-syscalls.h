/* Copyright (C) 2010 Texas Instruments Incorporated
 * Contributed by Mark Salter <msalter@redhat.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#ifdef __NR_pread64
#define __NR___syscall_pread __NR_pread64
static __always_inline _syscall5(ssize_t, __syscall_pread, int, fd, void *, buf,
			size_t, count, off_t, offset_hi, off_t, offset_lo);

static __always_inline ssize_t
_dl_pread(int fd, void *buf, size_t count, off_t offset)
{
  return(__syscall_pread(fd,buf,count, offset, offset >> 31));
}
#endif
