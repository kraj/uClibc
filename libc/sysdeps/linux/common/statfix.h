#ifndef STATFIX_H
#define STATFIX_H

/* Pull in whatever this particular arch's kernel thinks the kernel version of
 * struct stat should look like.  It turn out that each arch has a different
 * opinion on the subject, and different kernel revs use different names... */
#define stat kernel_stat
#define new_stat kernel_stat
#include <asm/stat.h> 
#undef new_stat
#undef stat

/* Now pull in libc's version of stat */
#define stat libc_stat
#include <sys/stat.h>
#undef stat

extern void statfix(struct libc_stat *libcstat, struct kernel_stat *kstat);
extern int _fxstat(int version, int fd, struct libc_stat * statbuf);

#endif

