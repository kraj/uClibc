#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

#include <features.h>

__BEGIN_DECLS

extern int      mount __P ((__const char* __specialfile,
                __const char* __dir,__const char* __filesystemype,
                unsigned long __rwflag,__const void *__data));

extern int      umount __P ((__const char* __specialfile));
                                        
                                        
__END_DECLS

#endif
