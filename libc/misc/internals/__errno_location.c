#include <errno.h>

#ifndef __UCLIBC_HAS_THREADS_NATIVE__
#undef errno
extern int errno;
#endif

int *
#ifndef __UCLIBC_HAS_THREADS_NATIVE__
weak_const_function
#endif
__errno_location (void)
{
    return &errno;
}

