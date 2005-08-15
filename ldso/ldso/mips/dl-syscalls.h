/* We can't use the real errno in ldso, since it has not yet
 * been dynamicly linked in yet. */
extern int _dl_errno;
#define __set_errno(X) {(_dl_errno) = (X);}
#include "sys/syscall.h"

#define MMAP_HAS_6_ARGS
