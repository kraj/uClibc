#ifndef STATFIX_H
#define STATFIX_H

#include <features.h>

#define	_FILE_OFFSET_BITS   64
#define __USE_FILE_OFFSET64
#define __USE_LARGEFILE64

#ifdef __UCLIBC_HAVE_LFS__

#include <sys/types.h>

/* Pull in whatever this particular arch's kernel thinks the kernel version of
 * struct stat should look like.  It turns out that each arch has a different
 * opinion on the subject, and different kernel revs use different names... */
#define stat kernel_stat
#define new_stat kernel_stat
#define stat64 kernel_stat64
#define new_stat64 kernel_stat64
#include <asm/stat.h> 
#undef new_stat64
#undef stat64
#undef new_stat
#undef stat


/* Now pull in libc's version of stat */
#define stat libc_stat
#define stat64 libc_stat64
#include <sys/stat.h>
#undef stat64
#undef stat

extern void statfix64(struct libc_stat64 *libcstat, struct kernel_stat64 *kstat);
extern int __fxstat64(int version, int fd, struct libc_stat64 * statbuf);

#endif /* __UCLIBC_HAVE_LFS__ */

#endif

