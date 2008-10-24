/* We can't use the real errno in ldso, since it has not yet
 * been dynamicly linked in yet. */
#include "sys/syscall.h"
extern int _dl_errno;
#undef __set_errno
#define __set_errno(X) {(_dl_errno) = (X);}

#if __GNUC_PREREQ (4, 1)
#warning !!! gcc 4.1 and later have problems with __always_inline so redefined as inline
# ifdef __always_inline
# undef __always_inline
# define __always_inline __inline__
# endif
#endif
