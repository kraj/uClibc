#ifndef _SYS_VFS_H
#define _SYS_VFS_H

#include <features.h>
#include <linux/vfs.h>

__BEGIN_DECLS

extern int __statfs __P ((__const char *__path, struct statfs *__buf));
extern int statfs __P ((__const char *__path, struct statfs *__buf));

extern int __fstatfs __P ((int __fildes, struct statfs *__buf));
extern int fstatfs __P ((int __fildes, struct statfs *__buf));

__END_DECLS

#endif /*_SYS_VFS_H */
