#include <errno.h>
#undef errno
libc_hidden_proto(errno)

/* psm: moved to bits/errno.h: libc_hidden_proto(__errno_location) */
int * weak_const_function __errno_location (void)
{
    return &errno;
}
#ifdef IS_IN_libc /* not really need, only to keep in sync w/ libc_hidden_proto */
libc_hidden_weak(__errno_location)
#endif
