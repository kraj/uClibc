#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <features.h>
#include <sys/types.h>
#include <linux/mman.h>

#ifndef MAP_ANON
#define MAP_ANON	MAP_ANONYMOUS	   /* idem */
#endif
#ifndef MAP_FILE
#define MAP_FILE	0x00	   /* The 'normal' way: mapped from file */
#endif

__BEGIN_DECLS

extern __ptr_t mmap __P((__ptr_t __addr, size_t __len,
		int __prot, int __flags, int __fd, off_t __off));
extern int munmap __P((__ptr_t __addr, size_t __len));
extern int mprotect __P ((__const __ptr_t __addr, size_t __len, int __prot));

extern int msync __P((__ptr_t __addr, size_t __len, int __flags));

extern int mlock __P((__const __ptr_t __addr, size_t __len));
extern int munlock __P((__const __ptr_t __addr, size_t __len));

extern int mlockall __P((int __flags));
extern int munlockall __P((void));

extern __ptr_t mremap __P((__ptr_t __addr, size_t __old_len,
		size_t __new_len, int __may_move));

__END_DECLS

#endif /* _SYS_MMAN_H */
