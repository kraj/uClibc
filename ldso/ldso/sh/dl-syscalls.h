/* We can't use the real errno in ldso, since it has not yet
 * been dynamicly linked in yet. */
#include "sys/syscall.h"
extern int _dl_errno;
#undef __set_errno
#define __set_errno(X) {(_dl_errno) = (X);}

#if __GNUC_PREREQ (4, 1) && !__GNUC_PREREQ (4, 5)
#warning !!! gcc 4.1 and later have problems with __always_inline so redefined as inline
/* but inlining doesn't work properly with GCC 4.5 without __always_inline.  */
# ifdef __always_inline
# undef __always_inline
# define __always_inline __inline__
# endif
#endif


#ifdef __NR_pread64             /* Newer kernels renamed but it's the same.  */
# ifdef __NR_pread
#  error "__NR_pread and __NR_pread64 both defined???"
# endif
# define __NR_pread __NR_pread64
#endif

#ifdef __NR_pread
#include "endian.h"

# define __NR___syscall_pread __NR_pread
static __inline__ _syscall6(ssize_t, __syscall_pread, int, fd, void *, buf,
		size_t, count, int, dummy, off_t, offset_hi, off_t, offset_lo)

static __always_inline
ssize_t _dl_pread(int fd, void *buf, size_t count, off_t offset)
{
	return(__syscall_pread(fd,buf,count,0,__LONG_LONG_PAIR(offset >> 31,offset)));
}
#endif /* __NR_pread */
